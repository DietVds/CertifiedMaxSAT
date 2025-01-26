#include "QMaxSATProoflogger.h"
#include "MaxSATProoflogger.hpp"

template int MaxSATProoflogger::add_unit_clause_blocking_literal<Lit>(Lit, int, Lit, long, bool);
template void MaxSATProoflogger::add_blocking_literal<Lit>(Lit, int);