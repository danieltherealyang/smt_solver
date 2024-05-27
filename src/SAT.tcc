#pragma once
#include "SAT.h"
#include <cassert>
#include <iostream>
#include <unordered_map>

template<typename T>
void SATClause<T>::add_literal(SATLiteral<T> l, SATPolarity p) {
  m_literal_map[l] = p;
}

template<typename T>
bool SATClause<T>::contains(const SATLiteral<T>& l) const {
  return m_literal_map.find(l) != m_literal_map.end();
}

template<typename T>
void SATClause<T>::print() const {
  for (const auto& i : m_literal_map) {
    std::cout << i.first.id << " " << ((i.second == POS) ? "TRUE" : "FALSE") << ',';
  }
  std::cout << std::endl;
}

template<typename T>
std::unordered_map<SATLiteral<T>, SATPolarity> SATClause<T>::get_map() const {
  return m_literal_map;
}

template<typename T>
SATClause<T> SATModel<T>::conflict_clause() {
  SATClause<T> result;
  for (const std::pair<SATLiteral<T>, bool>& p : m_assignments) {
    SATPolarity pol = (p.second) ? NEG : POS;
    result.add_literal(p.first, pol);
  }
  return result;
}

template<typename T>
void SATModel<T>::assign(SATLiteral<T> id, bool p) {
  m_assignments[id] = p;
}


