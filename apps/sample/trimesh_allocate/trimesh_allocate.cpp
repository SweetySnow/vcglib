/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2012                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/*! \file trimesh_allocate.cpp
\ingroup code_sample

\brief the minimal example of using the attributes

Attributes are a simple mechanism to associate user-defined 'attributes' to the simplicies and to the mesh.
\ref attributes for more Details
*/

#include<vcg/complex/complex.h>
#include<vcg/complex/algorithms/create/platonic.h>

class MyEdge;
class MyFace;
class MyVertex;
struct MyUsedTypes : public vcg::UsedTypes<	vcg::Use<MyVertex>		::AsVertexType,
                                            vcg::Use<MyFace>			::AsFaceType>{};

class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f,vcg::vertex::Normal3f>{};
class MyFace    : public vcg::Face< MyUsedTypes, vcg::face::VertexRef, vcg::face::Normal3f> {};

class MyMesh : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> > {};

int main()
{
  MyMesh m;
  vcg::tri::Allocator<MyMesh>::AddVertices(m,3);
  vcg::tri::Allocator<MyMesh>::AddFaces(m,1);

  MyMesh::VertexPointer ivp[3];
  MyMesh::VertexIterator vi=m.vert.begin();
  ivp[0]=&*vi;(*vi).P()=MyMesh::CoordType ( 0.0, 0.0, 0.0); ++vi;
  ivp[1]=&*vi;(*vi).P()=MyMesh::CoordType ( 1.0, 0.0, 0.0); ++vi;
  ivp[2]=&*vi;(*vi).P()=MyMesh::CoordType ( 0.0, 1.0, 0.0); ++vi;

  MyFace &f=m.face[0];
  f.V(0)=ivp[0];
  f.V(1)=ivp[1];
  f.V(2)=ivp[2];

  // a potentially dangerous pointer to a mesh element
  MyMesh::FacePointer fp = &m.face[0];
  vcg::tri::Allocator<MyMesh>::PointerUpdater<MyMesh::FacePointer> pu;

  // now the fp pointer could be no more valid due to eventual re-allocation of the m.face vector.
  vcg::tri::Allocator<MyMesh>::AddVertices(m,3);
  vcg::tri::Allocator<MyMesh>::AddFaces(m,1,pu);

  // check if an update of the pointer is needed and do it.
  if(pu.NeedUpdate()) pu.Update(fp);

  // Now fill the mesh with an Icosahedron and then delete some faces
  vcg::tri::Icosahedron(m);
  vcg::tri::Allocator<MyMesh>::DeleteFace(m,m.face[1]);
  vcg::tri::Allocator<MyMesh>::DeleteFace(m,m.face[3]);

  // If you loop in a mesh with deleted elements you have to skip them!
  MyMesh::FaceIterator fi;
  for(fi = m.face.begin(); fi!=m.face.end(); ++fi )
  {
     if(!fi->IsD()) //    <---- Check added
       {
        MyMesh::CoordType b = vcg::Barycenter(*fi);
       }
  }

  // WRONG WAY of iterating: FN() != m.face.size() if there are deleted elements
  for(int i=0;i<m.FN();++i)
  {
     if(!fi->IsD())
       {
        MyMesh::CoordType b = vcg::Barycenter(*fi);
       }
  }

  // finally lets copy this mesh onto another one.
  MyMesh m2;
  vcg::tri::Append<MyMesh,MyMesh>::MeshCopy(m2,m);

}