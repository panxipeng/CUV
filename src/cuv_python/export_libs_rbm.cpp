//*LB*
// Copyright (c) 2010, University of Bonn, Institute for Computer Science VI
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//  * Neither the name of the University of Bonn 
//    nor the names of its contributors may be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*LE*





#include <string>
#include <boost/python.hpp>
#include <boost/python/extract.hpp>
#include <pyublas/numpy.hpp>

#include <cuv/basics/tensor.hpp>
#include <cuv/libs/rbm/rbm.hpp>

//using namespace std;
using namespace boost::python;
using namespace cuv;
using namespace cuv::libs::rbm;
namespace ublas = boost::numeric::ublas;

template<class V, class M, class L>
void export_libs_rbm_detail(){
	def("set_binary_sequence", set_binary_sequence<V,M,L>, (arg("matrix"), arg("startvalue")));
	def("sigm_temperature", sigm_temperature<V,M,L>, (arg("matrix"), arg("temperature")));
}

template<class V, class M, class L>
void export_set_local_conn(){
	def("set_local_connectivity_in_dense_matrix", set_local_connectivity_in_dense_matrix<V,M,L>, (arg("matrix"),arg("patchsize"),arg("px"),arg("py"),arg("pxh"),arg("pyh"),arg("maxdist_from_main_dia"),arg("round")=false));
}

template<class V, class M, class L>
void export_copy_at_rowidx(){
	def("copy_at_rowidx", copy_at_rowidx<V,M,L>, (arg("dst"), arg("src"),arg("rowidx"),arg("offset")));
	def("copy_redblack", copy_redblack<V,M,L>, (arg("dst"), arg("src"),arg("num_maps"), arg("color")));
}

template <class M>
void export_bitflip(){
	typedef typename M::index_type I;
	//def("bitflip",(void(*)(M&,I))
			//bitflip<typename M::value_type, typename M::memory_layout,typename M::memory_space_type,typename M::index_type>);
}
void export_libs_rbm(){
	export_libs_rbm_detail<float,host_memory_space,column_major>();
	export_libs_rbm_detail<float,dev_memory_space,column_major>();
	export_set_local_conn<float,dev_memory_space,column_major>();
	export_copy_at_rowidx<float,dev_memory_space,column_major>();
	typedef tensor<float,dev_memory_space,column_major> fdev;
	typedef tensor<float,host_memory_space,column_major> fhost;
	export_bitflip<fdev>();
	export_bitflip<fhost>();
}
