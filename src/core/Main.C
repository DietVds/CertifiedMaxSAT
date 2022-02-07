/******************************************************************************************[Main.C]
QMaxSAT -- Copyright (c) 2010-2011, Miyuki Koshimura
MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include <ctime>
#include <cstring>
#include <stdint.h>
#include <errno.h>

#include <signal.h>
#include <zlib.h>

#include "Solver.h"
#include "Prooflogger.h"

/*************************************************************************************/
#ifdef _MSC_VER
#include <ctime>

static inline double cpuTime(void) {
    return (double)clock() / CLOCKS_PER_SEC; }
#else

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

static inline double cpuTime(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; }
#endif


#if defined(__linux__)
static inline int memReadStat(int field)
{
    char    name[256];
    pid_t pid = getpid();
    sprintf(name, "/proc/%d/statm", pid);
    FILE*   in = fopen(name, "rb");
    if (in == NULL) return 0;
    int     value;
    for (; field >= 0; field--)
        fscanf(in, "%d", &value);
    fclose(in);
    return value;
}
static inline uint64_t memUsed() { return (uint64_t)memReadStat(0) * (uint64_t)getpagesize(); }


#elif defined(__FreeBSD__)
static inline uint64_t memUsed(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss*1024; }


#else
static inline uint64_t memUsed() { return 0; }
#endif

#if defined(__linux__)
#include <fpu_control.h>
#endif

//=================================================================================================
// DIMACS Parser:

#define CHUNK_LIMIT 1048576

class StreamBuffer {
    gzFile  in;
    char    buf[CHUNK_LIMIT];
    int     pos;
    int     size;

    void assureLookahead() {
        if (pos >= size) {
            pos  = 0;
            size = gzread(in, buf, sizeof(buf)); } }

public:
    StreamBuffer(gzFile i) : in(i), pos(0), size(0) {
        assureLookahead(); }

    int  operator *  () { return (pos >= size) ? EOF : buf[pos]; }
    void operator ++ () { pos++; assureLookahead(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class B>
static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }

template<class B>
static void skipLine(B& in) {
    for (;;){
        if (*in == EOF || *in == '\0') return;
        if (*in == '\n') { ++in; return; }
        ++in; } }

template<class B>
static int parseInt(B& in) {
    int     val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') reportf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }

template<class B>
static void readClause(B& in, Solver& S, Prooflogger& PL, vec<Lit>& lits, 
		       int nbvar, int top, int& nbsoft) { // koshi 10.01.04

    int parsed_lit, var, weight;
    lits.clear();
    weight = parseInt(in); // koshi 10.01.04
    if (weight == 1) { // soft clause
      nbsoft++;
      lits.push(Lit(S.newVar()));
    } else if (weight != top) { // weight of hard clause must be top
      reportf("Unexpected weight %c\n", *in), exit(3);
    } else weight = top;

    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
	// koshi 10.01.04        while (var >= S.nVars()) S.newVar();
        lits.push( (parsed_lit > 0) ? Lit(var) : ~Lit(var) );
    }

    // Write the clause to the OPB file
    // PL.write_OPB_constraint(lits); // wcnf file read directly by veripb
}

template<class B>
static bool match(B& in, char* str) {
    for (; *str != 0; ++str, ++in)
        if (*str != *in)
            return false;
    return true;
}


template<class B>
static void parse_DIMACS_main(B& in, Solver& S, Prooflogger &PL, 
			      int& out_nbvar, int& out_top, int& out_nbsoft) {
    vec<Lit> lits;
    int vars, clauses;

    // Read header
    skipWhitespace(in);
    while(*in == 'c') skipLine(in);
    if (*in == 'p'){
	     if (match(in, "p wcnf")){ // koshi 10.01.04
            vars    = parseInt(in);            
            clauses = parseInt(in);            
		    int top     = parseInt(in);
		    out_nbvar   = vars;
		    out_top     = top;
            reportf("|  Number of variables:    %-12d                                       |\n", vars);
            reportf("|  Number of clauses:      %-12d                                       |\n", clauses);
            reportf("|  Weight of hard clauses: %-12d                                       |\n", top);
		    while (vars > S.nVars()) S.newVar();
        } else {
            reportf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
        }
    } else reportf("PARSE ERROR! No header given!");

    // Read clauses
    for (;;){
        skipWhitespace(in);
        if (*in == EOF)
            break;
        else if (*in == 'c' || *in == 'p') skipLine(in);
        else {
	        readClause(in, S, PL, lits, out_nbvar,out_top,out_nbsoft),
            S.addClause(lits);
        }
    }
    reportf("|  Number of soft clauses: %-12d                                       |\n", out_nbsoft);
    PL.n_variables = vars+out_nbsoft;
    PL.formula_length = clauses;
    PL.write_proof_header(clauses);
    
    // PL.write_OPB_header(vars, out_nbsoft, clauses); // wcnf file read directly by veripb
    // PL.write_minimise(out_nbvar, out_nbsoft);  // wcnf file read directly by veripb
}

// Inserts problem into solver.
//
static void parse_DIMACS(gzFile input_stream, Solver& S, Prooflogger &PL, 
			 int& out_nbvar, int& out_top, int& out_nbsoft) { // koshi 10.01.04
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S, PL, out_nbvar, out_top, out_nbsoft); }


//=================================================================================================


void printStats(Solver& solver)
{
    double   cpu_time = cpuTime();
    uint64_t mem_used = memUsed();
    reportf("restarts              : %lld\n", solver.starts);
    reportf("conflicts             : %-12lld   (%.0f /sec)\n", solver.conflicts   , solver.conflicts   /cpu_time);
    reportf("decisions             : %-12lld   (%4.2f %% random) (%.0f /sec)\n", solver.decisions, (float)solver.rnd_decisions*100 / (float)solver.decisions, solver.decisions   /cpu_time);
    reportf("propagations          : %-12lld   (%.0f /sec)\n", solver.propagations, solver.propagations/cpu_time);
    reportf("conflict literals     : %-12lld   (%4.2f %% deleted)\n", solver.tot_literals, (solver.max_literals - solver.tot_literals)*100 / (double)solver.max_literals);
    if (mem_used != 0) reportf("Memory used           : %.2f MB\n", mem_used / 1048576.0);
    reportf("CPU time              : %g s\n", cpu_time);
}

Solver* solver;
static void SIGINT_handler(int signum) {
    reportf("\n"); reportf("*** INTERRUPTED ***\n");
    printStats(*solver);
    reportf("\n"); reportf("*** INTERRUPTED ***\n");
    exit(1); }


//=================================================================================================
// Main:

void printUsage(char** argv)
{
    reportf("USAGE: %s [options] <input-file> <result-output-file>\n\n  where input may be either in plain or gzipped DIMACS.\n\n", argv[0]);
    reportf("OPTIONS:\n\n");
    reportf("  -polarity-mode       = {true,false,rnd}\n");
    reportf("  -decay               = <num> [ 0 - 1 ]\n");
    reportf("  -rnd-freq            = <num> [ 0 - 1 ]\n");
    reportf("  -verbosity           = {0,1,2}\n");
    reportf("  -proof-file          = /path/to/proof_file.proof (default: maxsat_proof.pbp)\n");
    reportf("  -problem-file        = /path/to/problem_file.opb (default: maxsat_problem.opb)\n");
    reportf("  -meaningful_names    = whether or not to assign meaningful names to the auxiliairy variables\n");
    reportf("\n");
}


const char* hasPrefix(const char* str, const char* prefix)
{
    int len = strlen(prefix);
    if (strncmp(str, prefix, len) == 0)
        return str + len;
    else
        return NULL;
}

// koshi 10.01.08
void genCardinals(int from, int to, 
		  Solver& S, Prooflogger& PL, vec<Lit>& lits, vec<Lit>& linkingVar) {
  int inputSize = to - from + 1;
  linkingVar.clear();

  vec<Lit> linkingAlpha;
  vec<Lit> linkingBeta;

  Var varZero = S.newVar();
  Var varLast = S.newVar();

  // First
  lits.clear(); lits.push(Lit(varZero)); S.addClause(lits);
  PL.write_unit_sub_red(lits, 0, from, to);

  // Last
  lits.clear(); lits.push(~Lit(varLast)); S.addClause(lits);
  PL.write_unit_sub_red(lits, inputSize+1, from, to);


  if (inputSize > 2) {
    int middle = inputSize/2;
    genCardinals(from, from+middle, S,PL,lits,linkingAlpha);
    genCardinals(from+middle+1, to, S,PL,lits,linkingBeta);
  } else if (inputSize == 2) {
    genCardinals(from, from, S,PL,lits,linkingAlpha);
    genCardinals(to, to, S,PL,lits,linkingBeta);
  }
  if (inputSize == 1) {
    linkingVar.push(Lit(varZero));
    linkingVar.push(Lit(from));
    linkingVar.push(Lit(varLast));
  } else { // inputSize >= 2

    PL.write_comment("- Node clauses:");
    linkingVar.push(Lit(varZero));
    for (int i = 0; i < inputSize; i++) linkingVar.push(Lit(S.newVar()));
    linkingVar.push(Lit(varLast));

    for (int sigma = 0; sigma <= inputSize; sigma++) {
        for (int alpha = 0; alpha < linkingAlpha.size()-1; alpha++) {
	        int beta = sigma - alpha;
	        if (0 <= beta && beta < linkingBeta.size()-1) { // create constraints
	            lits.clear();
	            lits.push(~linkingAlpha[alpha]);
	            lits.push(~linkingBeta[beta]);
	            lits.push(linkingVar[sigma]);
                PL.write_C1(lits, sigma, from, to);
	            S.addClause(lits);
	            lits.clear();
	            lits.push(linkingAlpha[alpha+1]);
	            lits.push(linkingBeta[beta+1]);
	            lits.push(~linkingVar[sigma+1]);
                PL.write_C2(lits, sigma+1, from, to);
	            S.addClause(lits);
	        }
        }
    }
    PL.write_comment("-------------------------------------------");
  }
  linkingAlpha.clear();
  linkingBeta.clear();
}

int main(int argc, char** argv)
{
    Prooflogger PL;
    Solver      S(&PL);
    S.verbosity = 1;

    int         i, j;
    const char* value;
    for (i = j = 0; i < argc; i++){
        if ((value = hasPrefix(argv[i], "-polarity-mode="))){
            if (strcmp(value, "true") == 0)
                S.polarity_mode = Solver::polarity_true;
            else if (strcmp(value, "false") == 0)
                S.polarity_mode = Solver::polarity_false;
            else if (strcmp(value, "rnd") == 0)
                S.polarity_mode = Solver::polarity_rnd;
            else{
                reportf("ERROR! unknown polarity-mode %s\n", value);
                exit(0); }

        }else if ((value = hasPrefix(argv[i], "-rnd-freq="))){
            double rnd;
            if (sscanf(value, "%lf", &rnd) <= 0 || rnd < 0 || rnd > 1){
                reportf("ERROR! illegal rnd-freq constant %s\n", value);
                exit(0); }
            S.random_var_freq = rnd;

        }else if ((value = hasPrefix(argv[i], "-decay="))){
            double decay;
            if (sscanf(value, "%lf", &decay) <= 0 || decay <= 0 || decay > 1){
                reportf("ERROR! illegal decay constant %s\n", value);
                exit(0); }
            S.var_decay = 1 / decay;

        }else if ((value = hasPrefix(argv[i], "-verbosity="))){
            int verbosity = (int)strtol(value, NULL, 10);
            if (verbosity == 0 && errno == EINVAL){
                reportf("ERROR! illegal verbosity level %s\n", value);
                exit(0); }
            S.verbosity = verbosity;

        }else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0){
            printUsage(argv);
            exit(0);

        } else if ((value = hasPrefix(argv[i], "-proof-file="))) {
            PL.set_proof_name(value);

        /*  // wcnf file read directly by veripb
        } else if ((value = hasPrefix(argv[i], "-opb-file="))) {
            PL.set_OPB_name(value);
        */
        }else if (strcmp(argv[i], "-mn") == 0 || strcmp(argv[i], "-meaningful_names") == 0 || strcmp(argv[i], "--meaningful_names") == 0){
            PL.meaningful_names = true;

        }else if (strncmp(argv[i], "-", 1) == 0){
            reportf("ERROR! unknown flag %s\n", argv[i]);
            exit(0);

        }else
            argv[j++] = argv[i];
    }
    argc = j;


    reportf("This is QMaxSAT 0.1 based on MiniSat 2.0 beta, extended for prooflogging\n");
#if defined(__linux__)
    fpu_control_t oldcw, newcw;
    _FPU_GETCW(oldcw); newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE; _FPU_SETCW(newcw);
    reportf("WARNING: for repeatability, setting FPU to use double precision\n");
#endif
    double cpu_time = cpuTime();

    solver = &S;
    signal(SIGINT,SIGINT_handler);
    signal(SIGHUP,SIGINT_handler);

    if (argc == 1)
        reportf("Reading from standard input... Use '-h' or '--help' for help.\n");

    gzFile in = (argc == 1) ? gzdopen(0, "rb") : gzopen(argv[1], "rb");
    if (in == NULL)
        reportf("ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]), exit(1);

    // Open OPB file 
    // PL.open_OPB_file(); // wcnf file read directly by veripb

    // Open proof file
    PL.open_proof_file();

    reportf("============================[ Problem Statistics ]=============================\n");
    reportf("|                                                                             |\n");

    // koshi 10.01.04 
    int nbvar  = 0; // number of original variables
    int top    = 0; // weight of hard clause
    int nbsoft = 0; // number of soft clauses
    parse_DIMACS(in, S, PL, nbvar, top, nbsoft);
    
    // Initialise PL constraint counter
    PL.constraint_counter = S.nClauses();

    // Close input file
    gzclose(in);

    // Write OPB constraints 
    //PL.write_OPB_file(); // wcnf file read directly by veripb

    // Open output file
    FILE* res = (argc >= 3) ? fopen(argv[2], "wb") : NULL;

    double parse_time = cpuTime() - cpu_time;
    reportf("|  Parsing time:         %-12.2f s                                       |\n", parse_time);

    if (!S.simplify()){
        reportf("Solved by unit propagation\n");
        if (res != NULL) fprintf(res, "UNSAT\n"), fclose(res);
        PL.write_empty_clause();
        PL.close_proof_file();
        printf("UNSATISFIABLE\n");
        exit(20);
    }

    int answer = nbsoft; // koshi 10.01.04
    vec<Lit> lits;
    int lcnt = 0; // loop count
    vec<Lit> linkingVar;
 solve:
    bool ret = S.solve();
    if (ret) { // koshi 09.12.25
      lcnt++;
      int answerNew = 0;
      for (int i = nbvar; i < nbvar+nbsoft; i++) if (S.model[i] == l_True) answerNew++;   // count the number ofunsatisfied soft clauses
      if (lcnt == 1) { // first model: generate cardinality constraints
          PL.write_comment("==============================================================");
          PL.write_comment("First model found:"); 
          PL.write_bound_update(S.model);
          PL.write_comment("==============================================================");
          PL.write_comment("Cardinality encoding:"); 
	       genCardinals(nbvar,nbvar+nbsoft-1, S,PL,lits,linkingVar);
          PL.write_comment("==============================================================");
          PL.write_comment("Tree derivation:"); 
          PL.write_tree_derivation();
          PL.write_comment("==============================================================");
          PL.write_comment("Constraining through linking variables:"); 
	       for (int i = answerNew; i < linkingVar.size()-1; i++) {
	         lits.clear();
	         lits.push(~linkingVar[i]);
             PL.write_linkingVar_clause(lits);
	         S.addClause(lits);
	       }
          PL.write_comment("==============================================================");
          answer = answerNew;
      } else { // lcnt > 1 
          PL.write_comment("==============================================================");
          PL.write_comment("New model found:"); 
          PL.write_bound_update(S.model);
          PL.write_comment("==============================================================");
          PL.write_comment("Constraining through linking variables:"); 
	       for (int i = answerNew; i < answer; i++) {
	           lits.clear();
	           lits.push(~linkingVar[i]);
               PL.write_linkingVar_clause(lits);
	           S.addClause(lits);
	       }
          PL.write_comment("==============================================================");
          answer = answerNew;
      }
      reportf("Current answer = %d\n",answer);
      goto solve;
    }
    printStats(S);
    reportf("\n");
    //    printf(ret ? "SATISFIABLE\n" : "UNSATISFIABLE\n");
    if (lcnt > 0 ) printf("Answer = %d by %d loops\n",answer,lcnt);
    else printf("Unsatisfiable\n");
    if (res != NULL){
        if (ret){
            fprintf(res, "SAT\n");
            for (int i = 0; i < S.nVars(); i++)
                if (S.model[i] != l_Undef)
                    fprintf(res, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
            fprintf(res, " 0\n");
        } else
            fprintf(res, "UNSAT\n");
        fclose(res);
    }
    PL.close_proof_file();

#ifdef NDEBUG
    exit(ret ? 10 : 20);     // (faster than "return", which will invoke the destructor for 'Solver')
#endif
}
