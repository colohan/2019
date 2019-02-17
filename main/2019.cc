#include <assert.h>
#include <iostream>
#include <math.h>
#include <set>
#include <sstream>
#include <string>

typedef int digit;
typedef std::multiset<digit> digit_set;

struct Expression {
  Expression(std::string s, double v) : asString(s), value(v) {}
  std::string asString;
  double value;
};

bool operator<(const Expression& a,
	       const Expression& b)
{
  return a.value < b.value;
}

typedef std::multiset<Expression> Expression_set;

// Given a set of digits, find all numbers which can be formed from (all of)
// them.
//
// E.g.:  {1,2,3} ==> {123,132,213,231,312,321}
Expression_set
digit_set_to_numbers(const digit_set& digits)
{
  Expression_set result;

  if (digits.size() == 1) {
    std::string num = std::to_string(*(digits.begin()));
    result.insert(Expression(num, std::stod(num)));

  } else {
    for(auto d = digits.begin(); d != digits.end(); d++) {
      digit_set digits_minus_d = digits;
      digits_minus_d.erase(digits_minus_d.find(*d));

      Expression_set remainder = digit_set_to_numbers(digits_minus_d);
      for(auto r = remainder.begin(); r != remainder.end(); r++) {
	std::string num = std::to_string(*d) + r->asString;
	result.insert(Expression(num, std::stod(num)));
      }
    }
  }

  return result;
}

std::string digit_set_as_string(const digit_set& digits)
{
  std::stringstream ss;
  ss << "{";
  for(auto d = digits.begin(); d != digits.end(); d++) {
    if (d != digits.begin()) {
      ss << ",";
    }
    ss << *d;
  }
  ss << "}";
  return ss.str();
}

void print_partition(const std::pair<digit_set,digit_set>& p)
{
  std::cout << digit_set_as_string(p.first) << ","
	    << digit_set_as_string(p.second) << std::endl;
}

// Given a set of digits, find all 2-way partitions of that set of digits
// assuming both partitions have at least one element.
//
// E.g.: {1,2,3}==>
// {1},{2,3}
// {2},{1,3}
// {3},{1,2}
// {2,3},{1}
// {1,3},{2}
// {1,2},{3}
std::multiset<std::pair<digit_set,digit_set>>
partition_digit_set(const digit_set& digits)
{
  assert(digits.size() >= 2);

  std::set<std::pair<digit_set,digit_set>> result;
  
  for(auto d = digits.begin(); d != digits.end(); d++) {
    digit_set digits_minus_d = digits;
    digits_minus_d.erase(digits_minus_d.find(*d));
    digit_set just_d;
    just_d.insert(*d);
    result.insert(std::pair<digit_set,digit_set>(just_d, digits_minus_d));

    if (digits_minus_d.size() >= 2) {
      std::multiset<std::pair<digit_set,digit_set>>
	minus_d_partitioned = partition_digit_set(digits_minus_d);
      for(auto p = minus_d_partitioned.begin();
	  p != minus_d_partitioned.end(); p++) {
	digit_set first(p->first.begin(), p->first.end());
	first.insert(*d);
	result.insert(std::pair<digit_set,digit_set>(first, p->second));
      }
    }
  }

  return std::multiset<std::pair<digit_set,digit_set>>(result.begin(),
						       result.end());
}

bool isInt(double v) {
  return fabs(v - round(v)) < 0.0000000001;
}

// Given a set of digits, find all possible expressions that can be built from
// them.  Result may be loooooong.
//
// E.g. If only considering the + operator then {1,2} ==> 
// 12
// 21
// 1+2
// 2+1
Expression_set
digit_set_to_expressions(const digit_set& digits)
{
  if (digits.size() >= 3) 
    std::cerr << "Generating numbers..."
	      << digit_set_as_string(digits) << std::endl;

  // First just consider the numbers which these digits can form:
  Expression_set numbers = digit_set_to_numbers(digits);

  if (digits.size() >= 3)
    std::cerr << "Adding decimal places numbers..."
	      << digit_set_as_string(digits) << std::endl;

  // Add in decimal places:
  Expression_set result = numbers;
  for(auto n = numbers.begin(); n != numbers.end(); n++) {
    if (n->value != 0.0) {
      std::string integer = n->asString;

      // Put decimal before first digit:
      std::string num = "." + integer;
      result.insert(Expression(num, std::stod(num)));

      // Put decimal between every pair of digits:
      for (auto i = integer.begin(); i != integer.end() - 1; i++) {
	std::string num = std::string(integer.begin(), i) + "." +
	  std::string(i,integer.end());
	result.insert(Expression(num, std::stod(num)));
      }
    }
  }  

  if (digits.size() >= 3)
    std::cerr << "Computing binary predicates..."
	      << digit_set_as_string(digits) << std::endl;

  // Now partition the set of digits into all 2-way partitions:
  if (digits.size() >= 2) {
    std::multiset<std::pair<digit_set,digit_set>> partitions =
      partition_digit_set(digits);
    for(auto p = partitions.begin(); p != partitions.end(); p++) {
      // For each partition, compute the complete set of expressions that can be
      // formed by the digits in that partition:
      Expression_set a_set = digit_set_to_expressions(p->first);
      Expression_set b_set = digit_set_to_expressions(p->second);

      for (auto a = a_set.begin(); a != a_set.end(); a++) {
	for (auto b = b_set.begin(); b != b_set.end(); b++) {
	  // Now we combine those two expressions using binary operators:

	  // operator+
	  result.insert(Expression("("+a->asString+")+("+b->asString+")",
				   a->value + b->value));

	  if (b->value != 0.0) {
	    // operator-
	    result.insert(Expression("("+a->asString+")-("+b->asString+")",
				   a->value - b->value));

	    // operator*
	    result.insert(Expression("("+a->asString+")*("+b->asString+")",
				     a->value * b->value));

	    // operator/
	    result.insert(Expression("("+a->asString+")/("+b->asString+")",
				     a->value / b->value));
	  }

	  // operator^
	  result.insert(Expression("("+a->asString+")^("+b->asString+")",
				   pow(a->value, b->value)));
	}
      }
    }
  }

  if (digits.size() >= 3)
    std::cerr << "Computing unary predicates..."
	      << digit_set_as_string(digits) << std::endl;

  Expression_set result_with_unary = result;
  for(auto r = result.begin(); r != result.end(); r++) {
    // Unary -
    if(r->value != 0.0) {
      result_with_unary.insert(Expression("-(" + r->asString + ")",
					  -r->value));
    }
    
    // Square root
    if(r->value != 1.0 && r->value > 0.0) {
      result_with_unary.insert(Expression("sqrt(" + r->asString + ")",
					  sqrt(r->value)));
    }

    // Cheating special case:  sqrt(9)!
    if(r->value == 9.0) {
      result_with_unary.insert(Expression("sqrt(" + r->asString + ")!",
					  6.0));
    }

    // Cube root
    if(r->value != 1.0 && r->value > 0.0) {
      result_with_unary.insert(Expression("cbrt(" + r->asString + ")",
					  std::cbrt(r->value)));
    }
    
    // Factorial
    if(r->value == 0.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 1.0));
    }
    if(r->value == 3.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 6.0));
    }
    if(r->value == 4.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 24.0));
    }
    if(r->value == 5.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 120.0));
    }
    if(r->value == 6.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 720.0));
    }
    if(r->value == 7.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 5040.0));
    }
    if(r->value == 8.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!", 40320.0));
    }
    // Probably not useful to have any bigger factorials

    // Double Factorial
    if(r->value == 0.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 1.0));
    }
    if(r->value == 1.0) {
      //result_with_unary.insert(Expression("(" + r->asString + ")!!", 1.0));
    }
    if(r->value == 2.0) {
      //result_with_unary.insert(Expression("(" + r->asString + ")!!", 2.0));
    }
    if(r->value == 3.0) {
      //result_with_unary.insert(Expression("(" + r->asString + ")!!", 3.0));
    }
    if(r->value == 4.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 8.0));
    }
    if(r->value == 5.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 15.0));
    }
    if(r->value == 6.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 48.0));
    }
    if(r->value == 7.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 105.0));
    }
    if(r->value == 8.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 384.0));
    }
    if(r->value == 9.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 945.0));
    }
    if(r->value == 10.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 3840.0));
    }
    if(r->value == 11.0) {
      result_with_unary.insert(Expression("(" + r->asString + ")!!", 10395.0));
    }

    // Nested factorial and double factorial
    if(r->value == 3.0) {
      result_with_unary.insert(Expression("((" + r->asString + ")!)!!", 48.0));
    }
  }
  
  if (digits.size() >= 3)
    std::cerr << "Optimizing memory..."
	      << digit_set_as_string(digits) << std::endl;

  // Blatant memory optimization.  Just prune any expression not super likely to
  // be helpful to us:
  result.clear();
  for(auto r = result_with_unary.begin(); r != result_with_unary.end(); r++) {
    if (isInt(r->value * 100.0) && r->value < 10000.0) {
      result.insert(*r);
    }
  }

  if (digits.size() >= 3)
    std::cerr << "Done, time for output..."
	      << digit_set_as_string(digits) << std::endl;

  return result;
}

int main()
{
  digit_set digits = { 2, 0, 1, 9 };

  Expression_set expr = digit_set_to_expressions(digits);
  
  for(auto e = expr.begin(); e != expr.end(); e++) {
    if (e->value > 0.9 && e->value <= 100.0 && isInt(e->value)) {
      std::cout << e->value << ": " << e->asString << std::endl;
    }
  }

  return 0;
}
