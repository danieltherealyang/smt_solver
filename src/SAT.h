#pragma once
#include <unordered_map>
#include <utility>
#include <vector>

enum SATPolarity {
  POS,
  NEG
};

template<typename T>
struct SATLiteral {
  T id;
  bool operator==(const SATLiteral& other) const {
    return id == other.id;
  }
};

namespace std {
  template<typename T>
  struct hash<SATLiteral<T>> {
    std::size_t operator()(const SATLiteral<T>& literal) const {
      return std::hash<T>()(literal.id);
    }
  };
}

template<typename T>
class SATClause {
private:
  std::unordered_map<SATLiteral<T>, SATPolarity> m_literal_map;
public:
  using iterator = std::unordered_map<SATLiteral<T>, SATPolarity>::iterator;
  using const_iterator = std::unordered_map<SATLiteral<T>, SATPolarity>::const_iterator;
  iterator begin() { return m_literal_map.begin(); }
  iterator end() { return m_literal_map.end(); }
  const_iterator begin() const { return m_literal_map.begin(); }
  const_iterator end() const { return m_literal_map.end(); }
  const_iterator cbegin() const { return m_literal_map.cbegin(); }
  const_iterator cend() const { return m_literal_map.cend(); }

  void add_literal(SATLiteral<T> l, SATPolarity p);
  bool contains(const SATLiteral<T>& l) const;
  std::unordered_map<SATLiteral<T>, SATPolarity> get_map() const;
  void print() const;
};

template<typename T>
class SATModel {
private:
  std::unordered_map<SATLiteral<T>, bool> m_assignments;
public:
  SATClause<T> conflict_clause();
  void assign(SATLiteral<T> id, bool p);
  using iterator = std::unordered_map<SATLiteral<T>, bool>::iterator;
  using const_iterator = std::unordered_map<SATLiteral<T>, bool>::const_iterator;
  iterator begin() { return m_assignments.begin(); }
  iterator end() { return m_assignments.end(); }
  const_iterator begin() const { return m_assignments.begin(); }
  const_iterator end() const { return m_assignments.end(); }
  const_iterator cbegin() const { return m_assignments.cbegin(); }
  const_iterator cend() const { return m_assignments.cend(); }
};

#include "SAT.tcc"
