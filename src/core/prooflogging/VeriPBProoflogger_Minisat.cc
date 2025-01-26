#include "QMaxSATProoflogger.h"
#include "VeriPBProoflogger.hpp"
//=================================================================================================

template void VeriPbProofLogger::add_objective_literal<Lit>(Lit&, long);
template void VeriPbProofLogger::removeReifiedConstraintRightImplFromConstraintStore<int>(int const&);
template int VeriPbProofLogger::getReifiedConstraintRightImpl<int>(int const&);
template void VeriPbProofLogger::removeReifiedConstraintLeftImplFromConstraintStore<int>(int const&);
template int VeriPbProofLogger::getReifiedConstraintLeftImpl<int>(int const&);
template int VeriPbProofLogger::reificationLiteralLeftImpl<vec<Lit>, Lit>(Lit const&, vec<Lit> const&, long, bool);
template int VeriPbProofLogger::reificationLiteralRightImpl<vec<Lit>, Lit>(Lit const&, vec<Lit> const&, long, bool);
template int VeriPbProofLogger::log_solution_lbools<vec<lbool> >(vec<lbool>&, long);
template void VeriPbProofLogger::delete_clause<Clause>(Clause const&, bool);
template int VeriPbProofLogger::rup<vec<Lit> >(vec<Lit> const&, long);
template void VeriPbProofLogger::store_meaningful_name<VeriPB::Var>(VeriPB::Var const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&);
template void VeriPbProofLogger::rewrite_variable_by_literal<VeriPB::Var, VeriPB::Lit>(VeriPB::Var const&, VeriPB::Lit const&);
template void VeriPbProofLogger::store_meaningful_name<int>(int const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&);
template void VeriPbProofLogger::rewrite_variable_by_literal<int, Lit>(int const&, Lit const&);
template int VeriPbProofLogger::rup<std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> > >(std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> > const&, long);
template void VeriPbProofLogger::add_boolean_assignment<VeriPB::Var>(std::pair<std::vector<std::pair<VeriPB::Var, VeriPB::Lit>, std::allocator<std::pair<VeriPB::Var, VeriPB::Lit> > >, std::vector<std::pair<VeriPB::Var, bool>, std::allocator<std::pair<VeriPB::Var, bool> > > >&, VeriPB::Var const&, bool);
template int VeriPbProofLogger::redundanceBasedStrengthening<std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> > >(std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> > const&, long, std::pair<std::vector<std::pair<VeriPB::Var, VeriPB::Lit>, std::allocator<std::pair<VeriPB::Var, VeriPB::Lit> > >, std::vector<std::pair<VeriPB::Var, bool>, std::allocator<std::pair<VeriPB::Var, bool> > > > const&);
template bool VeriPbProofLogger::remove_objective_literal<Lit>(Lit&);
template void VeriPbProofLogger::write_objective_update_diff<std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> >, std::vector<long, std::allocator<long> > >(std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> >&, std::vector<long, std::allocator<long> >&, long);
template void VeriPbProofLogger::delete_constraint<std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> > >(std::vector<VeriPB::Lit, std::allocator<VeriPB::Lit> > const&, long, std::pair<std::vector<std::pair<VeriPB::Var, VeriPB::Lit>, std::allocator<std::pair<VeriPB::Var, VeriPB::Lit> > >, std::vector<std::pair<VeriPB::Var, bool>, std::allocator<std::pair<VeriPB::Var, bool> > > > const&, bool);
template void VeriPbProofLogger::add_objective_literal<VeriPB::Lit>(VeriPB::Lit&, long);
template bool VeriPbProofLogger::remove_objective_literal<VeriPB::Lit>(VeriPB::Lit&);
