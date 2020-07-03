/*-------------------------------------------------------------------------*/
/* Copyright 2010-2020 Armin Biere Johannes Kepler University Linz Austria */
/*-------------------------------------------------------------------------*/

#ifndef lglib_h_INCLUDED
#define lglib_h_INCLUDED

#include <stdio.h>				// for 'FILE'
#include <stdlib.h>				// for 'int64_t'
#include <stdint.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

//--------------------------------------------------------------------------

#define LGL_UNKNOWN 0
#define LGL_SATISFIABLE 10
#define LGL_UNSATISFIABLE 20

//--------------------------------------------------------------------------

typedef struct LGL LGL;

//--------------------------------------------------------------------------

EXPORT LGL * lglinit (void);				// constructor
EXPORT void lglrelease (LGL *);			// destructor

// externally provided memory manager ...

typedef void * (*lglalloc) (void*mem, size_t);
typedef void (*lgldealloc) (void*mem, void*, size_t);
typedef void * (*lglrealloc) (void*mem, void *ptr, size_t old, size_t);

EXPORT LGL * lglminit (void *mem, lglalloc, lglrealloc, lgldealloc);

// 'Cloning' produces identicaly behaving solvers.

EXPORT LGL * lglclone (LGL *);
EXPORT LGL * lglmclone (LGL *, void *mem, lglalloc, lglrealloc, lgldealloc);

EXPORT int lglunclone (LGL * dst, LGL * src);		// does not release 'src'

// 'Forking' copies only irredundant clauses and also uses internal variable
// indices of the parent as external variable indices.  Thus 'parent' and
// the forked off 'child' do neither exactly work the same way, nor do they
// use from the API point of view the same set of variables.  They are
// satisfiability equivalent.  If the child becomes unsatisfiable the parent
// can be assumed to be unsatisfiable too and thus 'lgljoin' would just add
// the empty clause to the parent.  If the child produces a satisfying
// assignment, this assignment is turned into an assignment of the parent by
// 'lgljoin'.  Parents can be forked multiple times.  A child has exactly
// one parent.  Parents and children can be released independently.  Between
// 'lglfork' and 'lgljoin' no other operations but further 'lglfork' are
// allowed.  The effect ot multiple 'lgljoin' with the same parent is
// unclear at this point.  The same memory manager is use for the child and
// the parent. Options, prefix, output file and the callbacks for 'getime'
// and 'onabort' are copied too (if set).

EXPORT LGL * lglfork (LGL * parent);
EXPORT int lgljoin (LGL * parent, LGL * child);	// does not release 'child'

// Both 'Cloning' and 'Forking' can be used to implement 'Push & Pop', but
// the asymmetric forking is more similar to the classical way of
// implementing it, needs less resources since for instance eliminated
// variables do not occupy any memory in the children, but requires lifting
// back satisfying assignments explicitly (through the whole parent chain).
// If these full satisfying assignments are really needed actually cloning
// could be more space efficient too.  Assume you want to split the solver
// into two instances, one with a literal set to false, the other one to
// true. Then cloning allows you to produce two independent branches, while
// for forking you need three, since the root / top solver still has to be
// kept for lifting back a potential assignment.

//--------------------------------------------------------------------------

EXPORT const char * lglparsefile (LGL *, FILE *, int force,
                           int * lineno_ptr, int * max_var_ptr);

EXPORT const char * lglparsepath (LGL *, const char * path, int force,
                           int * lineno_ptr, int * max_var_ptr);

//--------------------------------------------------------------------------

EXPORT const char * lglversion ();

EXPORT void lglbnr (const char * name,
             const char * prefix,
	     FILE * file);			// ... banner

EXPORT void lglusage (LGL *);				// print usage "-h"
EXPORT void lglopts (LGL *, const char * prefix, int);	// ... defaults "-d" | "-e"
EXPORT void lglrgopts (LGL *);				// ... option ranges "-r"
EXPORT void lglpcs (LGL *, int mixed);			// ... PCS file
EXPORT void lglsizes (LGL *);				// ... data structure sizes

//--------------------------------------------------------------------------
// setters and getters for options

EXPORT void lglsetout (LGL *, FILE*);			// output file for report
EXPORT void lglsetrace (LGL *, FILE*);			// set trace output file
EXPORT void lglsetprefix (LGL *, const char*);		// prefix for messages

EXPORT FILE * lglgetout (LGL *);
EXPORT const char * lglgetprefix (LGL *);

EXPORT void lglsetopt (LGL *, const char *, int);	// set option value
EXPORT int lglreadopts (LGL *, FILE *);		// read and set options
EXPORT int lglgetopt (LGL *, const char *);		// get option value
EXPORT int lgldefopt (LGL *, const char *);		// get default value
EXPORT int lglhasopt (LGL *, const char *);		// exists option?

EXPORT int lglgetoptminmax (LGL *, const char *, int * minptr, int * maxptr);

EXPORT void * lglfirstopt (LGL *);			// option iterator: first

EXPORT void * lglnextopt (LGL *, 			// option iterator: next
                   void * iterator, 
                   const char ** nameptr,
		   int *valptr, int *minptr, int *maxptr);

// individual ids for logging and statistics:

EXPORT void lglsetid (LGL *, int tid, int tids);

// Set default phase of a literal.  Any decision on this literal will always
// try this phase.  Note, that this function does not have any effect on
// eliminated variables.  Further equivalent variables share the same forced 
// phase and thus if they are set to different default phases, only the last
// set operation will be kept.

EXPORT void lglsetphase (LGL *, int lit);
EXPORT void lglresetphase (LGL *, int lit);	// Stop forcing phase in decisions.

// Prefer decisions on 'important' variables.

EXPORT void lglsetimportant (LGL *, int lit);

// Assume the solver is in the SATISFIABLE state (after 'lglsat' or
// 'lglsimp'), then calling 'lglsetphases' will copy the current assignment
// as default phases.

EXPORT void lglsetphases (LGL *);

//--------------------------------------------------------------------------
// call back for abort

EXPORT void lglonabort (LGL *, void * state, void (*callback)(void* state));

//--------------------------------------------------------------------------
// write and read API trace

EXPORT void lglwtrapi (LGL *, FILE *);
EXPORT void lglrtrapi (LGL *, FILE *);

//--------------------------------------------------------------------------
// traverse units, equivalences, remaining clauses, or all clauses:

EXPORT void lglutrav (LGL *, void * state, void (*trav)(void *, int unit));
EXPORT void lgletrav (LGL *, void * state, void (*trav)(void *, int lit, int repr));
EXPORT void lglctrav (LGL *, void * state, void (*trav)(void *, int lit));
EXPORT void lgltravall (LGL *, void * state, void (*trav)(void *state, int lit));

//--------------------------------------------------------------------------

EXPORT void lglprint (LGL *, FILE *);			// remaining in DIMACS format
EXPORT void lglprintall (LGL *, FILE *);		// including units & equivs

//--------------------------------------------------------------------------
// main interface as in PicoSAT (see 'picosat.h' for more information)

EXPORT int lglmaxvar (LGL *);
EXPORT int lglincvar (LGL *);

EXPORT void lgladd (LGL *, int lit);
EXPORT void lglassume (LGL *, int lit);		// assume single units

EXPORT void lglcassume (LGL *, int lit);		// assume clause
						// (at most one)

EXPORT void lglfixate (LGL *);				// add assumptions as units

EXPORT int lglsat (LGL *);
EXPORT int lglsimp (LGL *, int iterations);

EXPORT int lglderef (LGL *, int lit);			// neg=false, pos=true
EXPORT int lglfixed (LGL *, int lit);			// ditto but toplevel

EXPORT int lglfailed (LGL *, int lit);			// ditto for assumptions
EXPORT int lglinconsistent (LGL *);			// contains empty clause?
EXPORT int lglchanged (LGL *);				// model changed

EXPORT void lglreducecache (LGL *);			// reset cache size
EXPORT void lglflushcache (LGL *);			// flush all learned clauses

/*------------------------------------------------------------------------*/

/* Return representative from equivalence class if literal is not top-level
 * assigned nor eliminated.
 */
EXPORT int lglrepr (LGL *, int lit);

/* Set 'startptr' and 'toptr' to the 'start' and 'top' of the reconstruction
 * stack, which is used in BCE, BVE and CCE for reconstructing a solution
 * after eliminating variables or clauses.  These pointers are only valid
 * until the next 'lglsat/lglsimp' call.
 */
EXPORT void lglreconstk (LGL * lgl, int ** startptr, int ** toptr);

//--------------------------------------------------------------------------
// Incremental interface provides reference counting for indices, i.e.
// unfrozen indices become invalid after next 'lglsat' (or 'lglsimp').
// This is actually a reference counter for variable indices still in use
// after the next 'lglsat' or 'lglsimp' call.  It is actually variable based
// and only applies to literals in new clauses or used as assumptions,
// e.g. in calls to 'lgladd' and 'lglassume'.
//
// The following example is actually compilable and used for explaining
// all the details of this rather complicated incremental API contract:

/***** start of incremental example ***************************************

#include "lglib.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>

EXPORT int main () {
  LGL * lgl = lglinit ();
  int res;
  lgladd (lgl, -14); lgladd (lgl, 2); lgladd (lgl, 0);  // binary clause
  lgladd (lgl, 14); lgladd (lgl, -1); lgladd (lgl, 0);  // binary clause
  lglassume (lgl, 1);                                   // assume '1'
  lglfreeze (lgl, 1);                                   // will use '1' below
  lglfreeze (lgl, 14);                                  // will use '14 too
  assert (lglfrozen (lgl, 1));
  assert (lglfrozen (lgl, 14));
  res = lglsat (lgl);
  assert (res == 10);
  (void) lglderef (lgl, 1);                             // fine
  (void) lglderef (lgl, 2);                             // fine
  (void) lglderef (lgl, 3);                             // fine !
  (void) lglderef (lgl, 14);                            // fine
  assert (!lglusable (lgl, 2));
  // lgladd (lgl, 2);                                   // ILLEGAL
  // lglassume (lgl, 2);                                // ILLEGAL
  assert (lglusable (lgl, 15));				// '15' not used yet!
  lgladd (lgl, -14); lgladd (lgl, 1); lgladd (lgl, 0);  // binary clause
  lgladd (lgl, 15); lgladd (lgl, 0);                    // fine!
  lglmelt (lgl, 14);                                    // '1' discarded
  res = lglsat (lgl);
  assert (res == 10);
  assert (lglfrozen (lgl, 1));
  (void) lglderef (lgl, 1);                             // fine
  (void) lglderef (lgl, 2);                             // fine
  (void) lglderef (lgl, 3);                             // fine
  (void) lglderef (lgl, 14);                            // fine
  (void) lglderef (lgl, 15);                            // fine
  assert (!lglusable (lgl, 2));
  assert (!lglusable (lgl, 14));
  // lglassume (lgl, 2);                                // ILLEGAL
  // lglassume (lgl, 14);                               // ILLEGAL
  lgladd (lgl, 1);                                      // still frozen
  lglmelt (lgl, 1);
  lgladd (lgl, 0);
  res = lglsat (lgl);
  assert (res == 10);					// TODO right?
  // none frozen
  assert (!lglusable (lgl, 1));
  // lgladd(lgl, 1);                                    // ILLEGAL
  lglsetopt (lgl, "plain", 1);				// disable BCE ...
  lgladd (lgl, 8); lgladd (lgl, -9); lgladd (lgl, 0);
  res = lglsat (lgl);
  assert (res == 10);
  assert (!lglusable (lgl, 8));
  assert (!lglusable (lgl, -9));
  assert (lglreusable (lgl, 8));
  assert (lglreusable (lgl, -9));
  lglreuse (lgl, 8);
  lgladd (lgl, -8); lgladd (lgl, 0);
  lglsetopt (lgl, "plain", 0);				// enable BCE ...
  res = lglsat (lgl);
  assert (res);
  return res;
}

****** end of incremental example ****************************************/

EXPORT void lglfreeze (LGL *, int lit);
EXPORT int lglfrozen (LGL *, int lit);

EXPORT void lglmelt (LGL *, int lit);
EXPORT void lglmeltall (LGL *);				// melt all literals

// If a literal was not frozen at the last call to 'lglsat' (or 'lglsimp')
// it becomes 'unusable' after the next call even though it might not
// have been used as blocking literal etc.  This

EXPORT int lglusable (LGL *, int lit);
EXPORT int lglreusable (LGL *, int lit);
EXPORT void lglreuse (LGL *, int lit);

//--------------------------------------------------------------------------
// Returns a good look ahead literal or zero if all potential literals have
// been eliminated or have been used as blocking literals in blocked clause
// elimination.  Zero is also returned if the formula is already
// inconsistent.  The lookahead literal is made usable again, for instance
// if it was not frozen during a previous SAT call and thus implicitly
// became melted.  Therefore it can be added in a unit clause.

EXPORT int lglookahead (LGL *);

//--------------------------------------------------------------------------
// stats:

EXPORT void lglflushtimers (LGL *lgl);			// after interrupt etc.

EXPORT void lglstats (LGL *);
EXPORT int64_t lglgetconfs (LGL *);
EXPORT int64_t lglgetdecs (LGL *);
EXPORT int64_t lglgetprops (LGL *);
EXPORT size_t lglbytes (LGL *);
EXPORT int lglnvars (LGL *);
EXPORT int lglnclauses (LGL *);
EXPORT double lglmb (LGL *);
EXPORT double lglmaxmb (LGL *);
EXPORT double lglsec (LGL *);
EXPORT double lglprocesstime (void);

//--------------------------------------------------------------------------
// low-level parallel support through call backs

EXPORT void lglseterm (LGL *, int (*term)(void*), void*);

EXPORT void lglsetproduceunit (LGL *, void (*produce)(void*, int), void*);
EXPORT void lglsetconsumeunits (LGL *, void (*consume)(void*,int**,int**), void*);
EXPORT void lglsetconsumedunits (LGL *, void (*consumed)(void*,int), void*);

EXPORT void lglsetproducecls (LGL*, void(*produce)(void*,int*,int glue),void*);
EXPORT void lglsetconsumecls (LGL*,void(*consume)(void*,int**,int *glueptr),void*);
EXPORT void lglsetconsumedcls (LGL *, void (*consumed)(void*,int), void*);

EXPORT void lglsetlockeq (LGL *, int * (*lock)(void*), void *);
EXPORT void lglsetunlockeq (LGL *, void (*unlock)(void*,int cons,int prod), void *);

EXPORT void lglsetmsglock (LGL *, void (*lock)(void*), void (*unlock)(void*), void*);
EXPORT void lglsetime (LGL *, double (*time)(void));

#endif
