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

#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include <cuv/tools/cuv_general.hpp>
#include <cuv/basics/tensor.hpp>
#include <cuv/basics/memory2d.hpp>
using namespace cuv;

struct MyConfig {
	static const int dev = CUDA_TEST_DEVICE;
	MyConfig()   { 
		printf("Testing on device=%d\n",dev);
		initCUDA(dev); 
	}
	~MyConfig()  { exitCUDA();  }
};

BOOST_GLOBAL_FIXTURE( MyConfig );

struct Fix{
	Fix()
	{
	}
	~Fix(){
	}
};

BOOST_FIXTURE_TEST_SUITE( s, Fix )

/** 
 * @test
 * @brief create tensor
 */
BOOST_AUTO_TEST_CASE( create_tensor )
{
	// column_major
	tensor<float,host_memory_space,column_major> m(extents[2][3][4]);
	BOOST_CHECK_EQUAL(24,m.size());
	BOOST_CHECK_EQUAL(2ul,m.shape()[0]);
	BOOST_CHECK_EQUAL(3ul,m.shape()[1]);
	BOOST_CHECK_EQUAL(4ul,m.shape()[2]);

	BOOST_CHECK_EQUAL(0ul,m.index_of(extents[0][0][0]));  // column major test
	BOOST_CHECK_EQUAL(1ul,m.index_of(extents[1][0][0]));
	BOOST_CHECK_EQUAL(2ul,m.index_of(extents[0][1][0]));


	// row_major
	tensor<float,host_memory_space,row_major> n(extents[2][3][4]);
	BOOST_CHECK_EQUAL(24,m.size());
	BOOST_CHECK_EQUAL(2ul,n.shape()[0]);
	BOOST_CHECK_EQUAL(3ul,n.shape()[1]);
	BOOST_CHECK_EQUAL(4ul,n.shape()[2]);

	BOOST_CHECK_EQUAL(0ul,n.index_of(extents[0][0][0]));  // row major test
	BOOST_CHECK_EQUAL(1ul,n.index_of(extents[0][0][1]));
	BOOST_CHECK_EQUAL(2ul,n.index_of(extents[0][0][2]));
	BOOST_CHECK_EQUAL(4ul,n.index_of(extents[0][1][0]));
}

BOOST_AUTO_TEST_CASE( tensor_data_access )
{
	tensor<float,host_memory_space,column_major> m(extents[2][3][4]);
	tensor<float,host_memory_space,row_major>    n(extents[2][3][4]);

	tensor<float,host_memory_space,column_major> o(extents[2][3][4]);
	tensor<float,host_memory_space,row_major>    p(extents[2][3][4]);
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 4; ++k) {
				m(i,j,k)=i*j+k;
				n(i,j,k)=i*j+k;

				o(i,j,k)=i*j+k;
				p(i,j,k)=i*j+k;
			}
		}
	}
	BOOST_CHECK_EQUAL(1*2+3,m(1,2,3));
	BOOST_CHECK_EQUAL(1*2+3,n(1,2,3));
	BOOST_CHECK_EQUAL(1*2+3,o(1,2,3));
	BOOST_CHECK_EQUAL(1*2+3,p(1,2,3));

	BOOST_CHECK_EQUAL(1*2+3-1,--p(1,2,3));
	BOOST_CHECK_EQUAL(1*2+3,  p(1,2,3)+=1);
}

BOOST_AUTO_TEST_CASE( tensor_assignment )
{
	tensor<float,host_memory_space,column_major> m(extents[2][3][4]);
	tensor<float,host_memory_space,column_major> n(extents[2][3][4]);

	tensor<float,host_memory_space,column_major> o(extents[2][3][4]);

	for (int i = 0; i < 2*3*4; ++i)
		m[i] = i;
	n = m;
	o = m;

	tensor<float,host_memory_space,column_major> s(n);
	tensor<float,dev_memory_space,column_major> t(n);

	for (int i = 0; i < 2*3*4; ++i){
		BOOST_CHECK_EQUAL(m[i], i);
		BOOST_CHECK_EQUAL(n[i], i);
		BOOST_CHECK_EQUAL(o[i], i);
		BOOST_CHECK_EQUAL(s[i], i);
		BOOST_CHECK_EQUAL(t[i], i);
	}


}

template<class V,class M1,class M2, class A1, class A2>
void test_pushpull_2d()
{
	static const int h=123,w=247;
	tensor<V,M1,row_major,A1> t1;
	tensor<V,M2,row_major,A2> t2(extents[h][w]);
	
	for(int i=0;i<h; i++)
		for(int j=0;j<w; j++){
			t2(i,j) = (float) drand48();
		}
	t1 = t2;
	BOOST_CHECK(equal_shape(t1,t2));
	for(int i=0;i<h; i++)
		for(int j=0;j<w; j++)
			BOOST_CHECK_EQUAL( (V) t1(i,j) , (V) t2(i,j) );
}
template<class V,class M1,class M2, class A1, class A2>
void test_pushpull_3d()
{
	static const int d=3,h=123,w=247;
	tensor<V,M1,row_major,A1> t1;
	tensor<V,M2,row_major,A2> t2(extents[d][h][w]);
	
	// ***************************************
	// assignment 2D --> 1D
	// ***************************************
	for(int k=0;k<d;k++)
		for(int i=0;i<h; i++)
			for(int j=0;j<w; j++){
				t2(k,i,j) = (float) drand48();
			}
	t1 = t2;
	BOOST_CHECK(equal_shape(t1,t2));
	for (int k = 0; k < d; ++k)
		for(int i=0;i<h; i++)
			for(int j=0;j<w; j++)
				BOOST_CHECK_EQUAL( (V) t1(k,i,j) , (V) t2(k,i,j) );

}

template<class V, class M>
void test_lowdim_views(){
	static const int d=3,h=123,w=247;
	tensor<V,M,row_major>              t1d(extents[d][h][w]);
	tensor<V,M,row_major,memory2d_tag> t2d(extents[d][h][w]);

	for(int k=0;k<d;k++)
		for(int i=0;i<h; i++)
			for(int j=0;j<w; j++){
				t2d(k,i,j) = (float) drand48();
			}

	// ***************************************
	// 2D View on 3D tensor
	// ***************************************
	for(int k=0;k<d;++k){
		tensor<V,M,row_major,memory2d_tag> view(indices[k][index_range(0,h)][index_range(0,w)], t2d);
		for(int i=0;i<h; i++)
			for(int j=0;j<w; j++)
				BOOST_CHECK_EQUAL( (V) view(i,j) , (V) t2d(k,i,j) );
	}

	// ***************************************
	// 1D View on 3D tensor
	// ***************************************
	for(int k=0;k<d;++k){
		for (int i = 0; i < h; ++i) {
		       tensor<V,M,row_major,linear_memory_tag> view(indices[k][i][index_range(0,w)], t2d);
		       for(int j=0;j<w; j++)
			      BOOST_REQUIRE_EQUAL( (V) view(j) , (V) t2d(k,i,j) );
		}
	}
}

BOOST_AUTO_TEST_CASE( lowdim_views ) {
	test_lowdim_views<float,host_memory_space>();
	test_lowdim_views<float,dev_memory_space>();
}
BOOST_AUTO_TEST_CASE( pushpull_nd )
{
	// same memory space, linear container
	test_pushpull_2d<float,host_memory_space,host_memory_space,linear_memory_tag,linear_memory_tag>();
	test_pushpull_2d<float, dev_memory_space, dev_memory_space,linear_memory_tag,linear_memory_tag>();

	// same memory space, 2d container
	test_pushpull_2d<float,host_memory_space,host_memory_space,memory2d_tag,memory2d_tag>();
	test_pushpull_2d<float, dev_memory_space, dev_memory_space,memory2d_tag,memory2d_tag>();

	// same memory space, 2d vs. 1d
	test_pushpull_2d<float,host_memory_space,host_memory_space,memory2d_tag,linear_memory_tag>();
	test_pushpull_2d<float, dev_memory_space, dev_memory_space,memory2d_tag,linear_memory_tag>();
	test_pushpull_2d<float,host_memory_space,host_memory_space,linear_memory_tag,memory2d_tag>();
	test_pushpull_2d<float, dev_memory_space, dev_memory_space,linear_memory_tag,memory2d_tag>();
}
BOOST_AUTO_TEST_CASE( tensor_view )
{
	linear_memory<float,host_memory_space,float*> l(100);
	const linear_memory<float,host_memory_space,float*> k(100, l.ptr(),true);
	BOOST_CHECK_EQUAL(l.ptr(), k.ptr());
	
	tensor<float,host_memory_space,column_major> a(extents[2][3][4]);
	tensor<float,host_memory_space,column_major> b(indices[2][3][4],a.ptr());
	BOOST_CHECK_EQUAL(a.ptr(), b.ptr());
}

BOOST_AUTO_TEST_SUITE_END()