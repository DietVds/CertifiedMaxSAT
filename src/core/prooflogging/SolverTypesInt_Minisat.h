//==========================================================================
// What is needed to integrate with the VeriPB prooflogging library:
// ------------------------------------------------------------------
// A type for the variable and for a literal. Suppose Var is the type for the variable and Lit for the literal. 
// Then, the following functions should be present:
//
// 1. Var variable(Lit l) - gets the variable out of the literal
// 2. bool is_negated(Lit l) - true if the literal is negated, false otherwise
// 3. Lit neg(Lit l) - returns a literal that is the negation of l
// 4. VeriPB::Var toVeriPbVar(Var v) - converts a variable to a VeriPB variable
// 5. VeriPB::Lit toVeriPbLit(Lit l) - converts a literal to a VeriPB literal
// 
// To use the TotalizerProoflogger:
// ------------------------------------------------------------------
// 6. Lit create_literal(Var var, bool negated) - Every Literal type should have a mkLit function that creates a literal for a given variable.
//==========================================================================
//prooflogging Library

#ifndef SolverTypes_Int_h
#define SolverTypes_Int_h

#include "SolverTypes.h"
#include "VeriPbSolverTypes.h"
#include "Vec.h"

// the type that is presenting weights in the solver. This should be defined by the solver developer.
#define wght int64_t
#define wght_max LLONG_MAX
#define signedWght int64_t 


inline Var variable(Lit l){return (var(l));}
inline bool is_negated(Lit l){return sign(l);}
inline Lit neg(Lit l){return ~l;};
inline Lit create_literal(Var var, bool negated){return Lit(var, negated);}

// Glucose's variables start from 0, whereas VeriPB counts their variables from 1. Therefore, the conversion needs to be done.
// Problem: if we would use them both, then it would be a problem. However, pysat already solves this by not using the 0 variable in the solvers.
inline VeriPB::Var toVeriPbVar(Var v){VeriPB::Var pbv; pbv.v= v+1; return pbv;}
inline VeriPB::Lit toVeriPbLit(Lit l){VeriPB::Lit pbl; pbl.negated = is_negated(l); pbl.v = toVeriPbVar(variable(l)); return pbl;}

// Glucose uses the LBool datatype. This should be able to be converted to boolean.
inline bool toBool(lbool v){return (toInt(v) == 0);}

// TODO: varidx function.

#endif