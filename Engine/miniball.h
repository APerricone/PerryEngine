//    Copright (C) 1999-2006, Bernd Gaertner
//    $Revision: 1.3 $
//    $Date: 2006/11/16 08:01:52 $
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA,
//    or download the License terms from prep.ai.mit.edu/pub/gnu/COPYING-2.0.
//
//    Contact:
//    --------
//    Bernd Gaertner
//    Institute of Theoretical Computer Science 
//    ETH Zuerich
//    CAB G32.2
//    CH-8092 Zuerich, Switzerland
//    http://www.inf.ethz.ch/personal/gaertner
//
// Modified by Perry to use Vector<> Class and float
// http://www.inf.ethz.ch/personal/gaertner/miniball.html
//    $Date: 2011/03/09 23:12:10 $

#include "vector.h"

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <list>
 
// Class Declarations
// ==================

// smallest enclosing ball of a set of n points in dimension d
template<typename type,int d>
class Miniball;

// smallest ball with a set of n <= d+1 points *on the boundary*
template<typename type,int d>
class Miniball_b; 

// Class Definitions
// =================

// Miniball
// --------
    
template<typename type,int d>
class Miniball 
{
public:
  // types
  typedef typename std::list<Vector<type,d> >::iterator         It;
  typedef typename std::list<Vector<type,d> >::const_iterator   Cit;
    
private:
  // data members
  std::list<Vector<type,d> > L;            // internal point set
  Miniball_b<type,d>        B;            // the current ball
  It                   support_end;  // past-the-end iterator of support set
    
  // private methods
  void        mtf_mb (It k);
  void        pivot_mb (It k);
  void        move_to_front (It j);
  type        max_excess (It t, It i, It& pivot) const;
    
public:
  // creates an empty ball
  Miniball() {}

  // copies p to the internal point set
  void        check_in (const Vector<type,d>& p);

  // builds the smallest enclosing ball of the internal point set
  void        build ();
    
  // returns center of the ball (undefined if ball is empty)
  Vector<type,d>       center() const;

  // returns squared_radius of the ball (-1 if ball is empty)
  type      squared_radius () const;

  // returns size of internal point set
  int         nr_points () const;

  // returns begin- and past-the-end iterators for internal point set
  Cit         points_begin () const;
  Cit         points_end () const;

  // returns size of support point set; this set has the following properties:
  // - there are at most d+1 support points, 
  // - all support points are on the boundary of the computed ball, and
  // - the smallest enclosing ball of the support point set equals the 
  //   smallest enclosing ball of the internal point set
  int         nr_support_points () const;

  // returns begin- and past-the-end iterators for internal point set
  Cit         support_points_begin () const;
  Cit         support_points_end () const;
    
  // assesses the quality of the computed ball. The return value is the
  // maximum squared distance of any support point or point outside the 
  // ball to the boundary of the ball, divided by the squared radius of
  // the ball. If everything went fine, this will be less than e-15 and
  // says that the computed ball approximately contains all the internal
  // points and has all the support points on the boundary.
  // 
  // The slack parameter that is set by the method says something about
  // whether the computed ball is really the *smallest* enclosing ball 
  // of the support points; if everything went fine, this value will be 0; 
  // a positive value may indicate that the ball is not smallest possible,
  // with the deviation from optimality growing with the slack
  type      accuracy (type& slack) const;

  // returns true if the accuracy is below the given tolerance and the
  // slack is 0
  bool        is_valid (type tolerance = 1e-15) const;
};
    

// Miniball_b
// ----------
    
template<typename type,int d>
class Miniball_b {
private:
  // data members
  int                 m, s;   // size and number of support points
  type              q0[d];
    
  type              z[d+1];
  type              f[d+1];
  type              v[d+1][d];
  type              a[d+1][d];
    
  type              c[d+1][d];
  type              sqr_r[d+1];
    
  type*             current_c;      // refers to some c[j]
  type              current_sqr_r;

public:
  Miniball_b() {reset();}
    
  // access
  const type*       center() const;
  type              squared_radius() const;
  int                 size() const;
  int                 support_size() const;
  type              excess (const Vector<type,d>& p) const;
    
  // modification
  void                reset(); // generates empty sphere with m=s=0
  bool                push (const Vector<type,d>& p);
  void                pop ();
    
  // checking
  type              slack() const;
};


// Class Implementations
// =====================
    
// Miniball
// --------
   
template<typename type,int d>
void Miniball<type,d>::check_in (const Vector<type,d>& p)
{
  L.push_back(p);
}
   
   
template<typename type,int d>
void Miniball<type,d>::build ()
{
  B.reset();
  support_end = L.begin();
  pivot_mb (L.end());
}
   
   
template<typename type,int d>
void Miniball<type,d>::mtf_mb (It i)
{
  support_end = L.begin();
  if ((B.size())==d+1) return;
  for (It k=L.begin(); k!=i;) {
    It j=k++;
    if (B.excess(*j) > 0) {
      if (B.push(*j)) {
	mtf_mb (j);
	B.pop();
	move_to_front(j);
      }
    }
  }
}
   
template<typename type,int d>
void Miniball<type,d>::move_to_front (It j)
{
  if (support_end == j)
    ++support_end;
  L.splice (L.begin(), L, j);
}
   
   
template<typename type,int d>
void Miniball<type,d>::pivot_mb (It i)
{
  It t = ++L.begin();
  mtf_mb (t);
  type max_e, old_sqr_r = -1;
  do {
    It pivot;
    max_e = max_excess (t, i, pivot);
    if (max_e > 0) {
      t = support_end;
      if (t==pivot) ++t;
      old_sqr_r = B.squared_radius();
      B.push (*pivot);
      mtf_mb (support_end);
      B.pop();
      move_to_front (pivot);
    }
  } while ((max_e > 0) && (B.squared_radius() > old_sqr_r));
}
   
   
template<typename type,int d>
type Miniball<type,d>::max_excess (It t, It i, It& pivot) const
{
  const type *c = B.center(), sqr_r = B.squared_radius();
  type e, max_e = 0;
  for (It k=t; k!=i; ++k) {
    e = -sqr_r;
    for (int j=0; j<d; ++j)
      e += ((*k)[j]-c[j])*((*k)[j]-c[j]);
    if (e > max_e) {
      max_e = e;
      pivot = k;
    }
  }
  return max_e;
}
   
   
   
template<typename type,int d>
Vector<type,d> Miniball<type,d>::center () const
{
  return Vector<type,d>(B.center());
}
   
template<typename type,int d>
type Miniball<type,d>::squared_radius () const
{
  return B.squared_radius();
}
   
   
template<typename type,int d>
int Miniball<type,d>::nr_points () const
{
  return L.size();
}
   
template<typename type,int d>
typename Miniball<type,d>::Cit Miniball<type,d>::points_begin () const
{
  return L.begin();
}
   
template<typename type,int d>
typename Miniball<type,d>::Cit Miniball<type,d>::points_end () const
{
  return L.end();
}
   
   
template<typename type,int d>
int Miniball<type,d>::nr_support_points () const
{
  return B.support_size();
}
   
template<typename type,int d>
typename Miniball<type,d>::Cit Miniball<type,d>::support_points_begin () const
{
  return L.begin();
}
   
template<typename type,int d>
typename Miniball<type,d>::Cit Miniball<type,d>::support_points_end () const
{
  return support_end;
}
   
   
   
template<typename type,int d>
type Miniball<type,d>::accuracy (type& slack) const
{
  type e, max_e = 0;
  int n_supp=0;
  Cit i;
  for (i=L.begin(); i!=support_end; ++i,++n_supp)
    if ((e = std::abs (B.excess (*i))) > max_e)
      max_e = e;
   
  // you've found a non-numerical problem if the following ever fails
  assert (n_supp == nr_support_points());
   
  for (i=support_end; i!=L.end(); ++i)
    if ((e = B.excess (*i)) > max_e)
      max_e = e;
   
  slack = B.slack();
  return (max_e/squared_radius());
}
   
   
template<typename type,int d>
bool Miniball<type,d>::is_valid (type tolerance) const
{
  type slack;
  return ( (accuracy (slack) < tolerance) && (slack == 0) );
}
   
   

// Miniball_b
// ----------
   
template<typename type,int d>
const type* Miniball_b<type,d>::center () const
{
  return current_c;
}
   
template<typename type,int d>
type Miniball_b<type,d>::squared_radius() const
{
  return current_sqr_r;
}
   
template<typename type,int d>
int Miniball_b<type,d>::size() const
{
  return m;
}
   
template<typename type,int d>
int Miniball_b<type,d>::support_size() const
{
  return s;
}
   
template<typename type,int d>
type Miniball_b<type,d>::excess (const Vector<type,d>& p) const
{
  type e = -current_sqr_r;
  for (int k=0; k<d; ++k)
    e += (p[k]-current_c[k])*(p[k]-current_c[k]);
  return e;
}
   
   
   
template<typename type,int d>
void Miniball_b<type,d>::reset ()
{
  m = s = 0;
  // we misuse c[0] for the center of the empty sphere
  for (int j=0; j<d; ++j)
    c[0][j]=0;
  current_c = c[0];
  current_sqr_r = -1;
}
   
   
template<typename type,int d>
void Miniball_b<type,d>::pop ()
{
  --m;
}
   
   
template<typename type,int d>
bool Miniball_b<type,d>::push (const Vector<type,d>& p)
{
  int i, j;
  type eps = (type)(1e-32);
  if (m==0) {
    for (i=0; i<d; ++i)
      q0[i] = p[i];
    for (i=0; i<d; ++i)
      c[0][i] = q0[i];
    sqr_r[0] = 0;
  } else {
    // set v_m to Q_m
    for (i=0; i<d; ++i)
      v[m][i] = p[i]-q0[i];
   
    // compute the a_{m,i}, i< m
    for (i=1; i<m; ++i) {
      a[m][i] = 0;
      for (j=0; j<d; ++j)
	a[m][i] += v[i][j] * v[m][j];
      a[m][i]*=(2/z[i]);
    }
   
    // update v_m to Q_m-\bar{Q}_m
    for (i=1; i<m; ++i) {
      for (j=0; j<d; ++j)
	v[m][j] -= a[m][i]*v[i][j];
    }
   
    // compute z_m
    z[m]=0;
    for (j=0; j<d; ++j)
      z[m] += (v[m][j]*v[m][j]);
    z[m]*=2;
   
    // reject push if z_m too small
    if (z[m]<eps*current_sqr_r) {
      return false;
    }
   
    // update c, sqr_r
    type e = -sqr_r[m-1];
    for (i=0; i<d; ++i)
      e += (p[i]-c[m-1][i])*(p[i]-c[m-1][i]);
    f[m]=e/z[m];
   
    for (i=0; i<d; ++i)
      c[m][i] = c[m-1][i]+f[m]*v[m][i];
    sqr_r[m] = sqr_r[m-1] + e*f[m]/2;
  }
  current_c = c[m];
  current_sqr_r = sqr_r[m];
  s = ++m;
  return true;
}
      
template<typename type,int d>
type Miniball_b<type,d>::slack () const
{
  type l[d+1], min_l=0;
  l[0] = 1;
  for (int i=s-1; i>0; --i) {
    l[i] = f[i];
    for (int k=s-1; k>i; --k)
      l[i]-=a[k][i]*l[k];
    if (l[i] < min_l) min_l = l[i];
    l[0] -= l[i];
  }
  if (l[0] < min_l) min_l = l[0];
  return ( (min_l < 0) ? -min_l : 0);
}
   