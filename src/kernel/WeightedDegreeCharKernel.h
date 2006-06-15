/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2006 Soeren Sonnenburg
 * Written (W) 1999-2006 Gunnar Raetsch
 * Written (W) 1999-2006 Fabio De Bona
 * Copyright (C) 1999-2006 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#ifndef _WEIGHTEDDEGREECHARKERNEL_H___
#define _WEIGHTEDDEGREECHARKERNEL_H___

#include "lib/common.h"
#include "kernel/CharKernel.h"

#ifdef USE_TREEMEM
#define NO_CHILD ((INT)-1) 
#else
#define NO_CHILD NULL
#endif

class CWeightedDegreeCharKernel: public CCharKernel
{
  public:
	struct Trie
	{
		DREAL weight;
		union 
		{
			SHORTREAL child_weights[4];
#ifdef USE_TREEMEM
			INT children[4];
#else
			struct Trie *children[4];
#endif
		};
	};

 public:
  CWeightedDegreeCharKernel(LONG size, DREAL* weights, INT degree, INT max_mismatch, bool use_normalization=true, bool block_computation=false, INT mkl_stepsize=1) ;
  ~CWeightedDegreeCharKernel() ;
  
  virtual bool init(CFeatures* l, CFeatures* r, bool do_init);
  virtual void cleanup();

  /// load and save kernel init_data
  bool load_init(FILE* src);
  bool save_init(FILE* dest);

  // return what type of kernel we are Linear,Polynomial, Gaussian,...
  virtual EKernelType get_kernel_type() { return K_WEIGHTEDDEGREE; }

  // return the name of a kernel
  virtual const CHAR* get_name() { return "WeightedDegree" ; } ;

  inline virtual bool init_optimization(INT count, INT *IDX, DREAL* weights)
  {
	  return init_optimization(count, IDX, weights, -1);
  }

  virtual bool init_optimization(INT count, INT *IDX, DREAL* weights, INT tree_num);
  virtual bool delete_optimization() ;
  virtual DREAL compute_optimized(INT idx) 
  { 
    if (get_is_initialized())
      return compute_by_tree(idx); 
    
    CIO::message(M_ERROR, "CWeightedDegreeCharKernel optimization not initialized\n") ;
    return 0 ;
  } ;

  virtual DREAL* compute_batch(INT& num_vec, DREAL* target, INT num_suppvec, INT* IDX, DREAL* weights, DREAL factor);

  // subkernel functionality
  inline virtual void clear_normal()
  {
	  if (get_is_initialized())
	  {
		  delete_tree(NULL); 
		  set_is_initialized(false);
	  }
  }
  inline virtual void add_to_normal(INT idx, DREAL weight) 
  {
	  if (max_mismatch==0)
		  add_example_to_tree(idx, weight);
	  else
		  add_example_to_tree_mismatch(idx, weight);

	  set_is_initialized(true);
  }
  inline virtual INT get_num_subkernels()
  {
	  if (position_weights!=NULL)
		  return (INT) ceil(1.0*seq_length/mkl_stepsize) ;
	  if (length==0)
		  return (INT) ceil(1.0*get_degree()/mkl_stepsize);
	  return (INT) ceil(1.0*get_degree()*length/mkl_stepsize) ;
  }
  inline void compute_by_subkernel(INT idx, DREAL * subkernel_contrib)
  { 
	  if (get_is_initialized())
	  {
		  compute_by_tree(idx, subkernel_contrib); 
		  return ;
	  }
	  CIO::message(M_ERROR, "CWeightedDegreePositionCharKernel optimization not initialized\n") ;
  } ;
  inline const DREAL* get_subkernel_weights(INT& num_weights)
  {
	  num_weights = get_num_subkernels() ;

	  delete[] weights_buffer ;
	  weights_buffer = new DREAL[num_weights] ;

	  if (position_weights!=NULL)
		  for (INT i=0; i<num_weights; i++)
			  weights_buffer[i] = position_weights[i*mkl_stepsize] ;
	  else
		  for (INT i=0; i<num_weights; i++)
			  weights_buffer[i] = weights[i*mkl_stepsize] ;

	  return weights_buffer ;
  }
  inline void set_subkernel_weights(DREAL* weights2, INT num_weights2)
  {
	  INT num_weights = get_num_subkernels() ;
	  if (num_weights!=num_weights2)
		  CIO::message(M_ERROR, "number of weights do not match\n") ;

	  if (position_weights!=NULL)
	  {
		  for (INT i=0; i<num_weights; i++)
		  {
			  for (INT j=0; j<mkl_stepsize; j++)
			  {
				  if (i*mkl_stepsize+j<seq_length)
					  position_weights[i*mkl_stepsize+j] = weights2[i] ;
			  }
		  }
	  }
	  else if (length==0)
	  {
		  for (INT i=0; i<num_weights; i++)
		  {
			  for (INT j=0; j<mkl_stepsize; j++)
			  {
				  if (i*mkl_stepsize+j<get_degree())
					  weights[i*mkl_stepsize+j] = weights2[i] ;
			  }
		  }
	  }
	  else
	  {
		  for (INT i=0; i<num_weights; i++)
		  {
			  for (INT j=0; j<mkl_stepsize; j++)
			  {
				  if (i*mkl_stepsize+j<get_degree()*length)
					  weights[i*mkl_stepsize+j] = weights2[i] ;
			  }
		  }
	  }
  }
  
  // other kernel tree operations  
  DREAL *compute_abs_weights(INT & len);
  DREAL compute_abs_weights_tree(struct Trie * p_tree);
  void compute_by_tree(INT idx, DREAL *LevelContrib);

  bool is_tree_initialized() { return tree_initialized; }

  inline INT get_max_mismatch() { return max_mismatch; }
  inline INT get_degree() { return degree; }
  inline DREAL *get_degree_weights(INT& d, INT& len)
  {
	  d=degree;
	  len=length;
	  return weights;
  }
  inline DREAL *get_weights(INT& num_weights)
  {
	  if (position_weights!=NULL)
	  {
		  num_weights = seq_length ;
		  return position_weights ;
	  }
	  if (length==0)
		  num_weights = degree ;
	  else
		  num_weights = degree*length ;
	  return weights;
  }
  inline DREAL *get_position_weights(INT& len)
  {
	  len=seq_length;
	  return position_weights;
  }

  bool set_weights(DREAL* weights, INT d, INT len=0);
  bool set_position_weights(DREAL* position_weights, INT len=0);
  bool init_matching_weights_wd();
  bool delete_position_weights() { delete[] position_weights ; position_weights=NULL ; return true ; } ;

 protected:

  void add_example_to_tree(INT idx, DREAL weight);
  void add_example_to_single_tree(INT idx, DREAL weight, INT tree_num);
  void add_example_to_tree_mismatch(INT idx, DREAL weight);
  void add_example_to_single_tree_mismatch(INT idx, DREAL weight, INT tree_num);
  void add_example_to_tree_mismatch_recursion(struct Trie *tree,  DREAL alpha,
											  INT *vec, INT len_rem, 
											  INT depth_rec, INT mismatch_rec) ;
  
  DREAL compute_by_tree(INT idx);
  void delete_tree(struct Trie * p_tree=NULL);

  /// compute kernel function for features a and b
  /// idx_{a,b} denote the index of the feature vectors
  /// in the corresponding feature object
  DREAL compute(INT idx_a, INT idx_b);
  /*    compute_kernel*/
  DREAL compute_with_mismatch(CHAR* avec, INT alen, CHAR* bvec, INT blen) ;
  DREAL compute_without_mismatch(CHAR* avec, INT alen, CHAR* bvec, INT blen) ;
  DREAL compute_without_mismatch_matrix(CHAR* avec, INT alen, CHAR* bvec, INT blen) ;
  DREAL compute_using_block(CHAR* avec, INT alen, CHAR* bvec, INT blen);

  virtual void remove_lhs() ;
  virtual void remove_rhs() ;

  DREAL compute_by_tree_helper(INT* vec, INT len, INT pos) ;
  void compute_by_tree_helper(INT* vec, INT len, INT pos, DREAL* LevelContrib, DREAL factor) ;

#ifdef USE_TREEMEM
	inline void check_treemem()
	{
		if (TreeMemPtr+10>=TreeMemPtrMax) 
		{
			CIO::message(M_DEBUG, "Extending TreeMem from %i to %i elements\n", TreeMemPtrMax, (INT) ((double)TreeMemPtrMax*1.2)) ;
			TreeMemPtrMax = (INT) ((double)TreeMemPtrMax*1.2) ;
			TreeMem = (struct Trie *)realloc(TreeMem,TreeMemPtrMax*sizeof(struct Trie)) ;

			if (!TreeMem)
				CIO::message(M_ERROR, "out of memory\n");
		}
	}
#endif

 protected:
  ///degree*length weights
  ///length must match seq_length if != 0
  DREAL* weights;
  DREAL* position_weights ;
  DREAL* weights_buffer ;
  INT mkl_stepsize ;
  INT degree;
  INT length;
  
  INT max_mismatch ;
  INT seq_length ;

  double* sqrtdiag_lhs;
  double* sqrtdiag_rhs;

  bool initialized ;

  struct Trie **trees ;
  bool tree_initialized ;
  bool use_normalization ;
  bool block_computation;

  DREAL* matching_weights;

#ifdef USE_TREEMEM
	struct Trie* TreeMem ;
	INT TreeMemPtr ;
	INT TreeMemPtrMax ;
#endif
};

/* computes the simple kernel */
inline DREAL CWeightedDegreeCharKernel::compute_by_tree_helper(INT* vec, INT len, INT pos)
{
	DREAL sum=0 ;

	struct Trie *tree = trees[pos] ;
	ASSERT(tree!=NULL) ;

	for (INT j=0; pos+j < len; j++)
	{
		if ((j<degree-1) && (tree->children[vec[pos+j]]!=NO_CHILD))
		{
#ifdef USE_TREEMEM
			tree=&TreeMem[tree->children[vec[pos+j]]];
#else
			tree=tree->children[vec[pos+j]];
#endif
			sum += tree->weight;
		}
		else
		{
			if (j==degree-1)
				sum += tree->child_weights[vec[pos+j]];
			break;
		}
	} 

	if (position_weights!=NULL)
		return sum*position_weights[pos] ;
	else
		return sum ;
}

inline void CWeightedDegreeCharKernel::compute_by_tree_helper(INT* vec, INT len, INT pos, DREAL* LevelContrib, DREAL factor) 
{
	struct Trie *tree = trees[pos] ;
	ASSERT(tree!=NULL) ;
	if (factor==0)
		return ;

	if (position_weights!=NULL)
	{
		factor *= position_weights[pos] ;
		if (factor==0)
			return ;
		if (length==0) // with position_weigths, weights is a vector (1 x degree)
		{
			for (INT j=0; pos+j<len; j++)
			{
				if ((j<degree-1) && (tree->children[vec[pos+j]]!=NO_CHILD))
				{
#ifdef USE_TREEMEM
					tree=&TreeMem[tree->children[vec[pos+j]]];
#else
					tree=tree->children[vec[pos+j]];
#endif
					LevelContrib[pos/mkl_stepsize] += factor*tree->weight*weights[j] ;
				} 
				else
				{
					if (j==degree-1)
						LevelContrib[pos/mkl_stepsize] += factor*tree->child_weights[vec[pos+j]]*weights[j] ;
				}
			}
		} 
		else // with position_weigths, weights is a matrix (len x degree)
		{
			for (INT j=0; pos+j<len; j++)
			{
				if ((j<degree-1) && (tree->children[vec[pos+j]]!=NO_CHILD))
				{
#ifdef USE_TREEMEM
					tree=&TreeMem[tree->children[vec[pos+j]]];
#else
					tree=tree->children[vec[pos+j]];
#endif
					LevelContrib[pos/mkl_stepsize] += factor*tree->weight*weights[j+pos*degree] ;
				} 
				else
				{
					if (j==degree-1)
						LevelContrib[pos/mkl_stepsize] += factor*tree->child_weights[vec[pos+j]]*weights[j+pos*degree] ;

					break ;
				}
			}
		} 
	}
	else if (length==0) // no position_weigths, weights is a vector (1 x degree)
	{
		for (INT j=0; pos+j<len; j++)
		{
			if ((j<degree-1) && (tree->children[vec[pos+j]]!=NO_CHILD))
			{
#ifdef USE_TREEMEM
				tree=&TreeMem[tree->children[vec[pos+j]]];
#else
				tree=tree->children[vec[pos+j]];
#endif
				LevelContrib[j/mkl_stepsize] += factor*tree->weight*weights[j] ;
			}
			else
			{
				if (j==degree-1)
					LevelContrib[j/mkl_stepsize] += factor*tree->child_weights[vec[pos+j]]*weights[j] ;
				break ;
			}
		} 
	} 
	else // no position_weigths, weights is a matrix (len x degree)
	{
		for (INT j=0; pos+j<len; j++)
		{
			if ((j<degree-1) && (tree->children[vec[pos+j]]!=NO_CHILD))
			{
#ifdef USE_TREEMEM
				tree=&TreeMem[tree->children[vec[pos+j]]];
#else
				tree=tree->children[vec[pos+j]];
#endif
				LevelContrib[(j+degree*pos)/mkl_stepsize] += factor * tree->weight * weights[j+pos*degree] ;
			}
			else
			{
				if (j==degree-1)
					LevelContrib[(j+degree*pos)/mkl_stepsize] += factor * tree->child_weights[vec[pos+j]] * weights[j+pos*degree] ;
				break ;
			}
		} 
	}
}
#endif
