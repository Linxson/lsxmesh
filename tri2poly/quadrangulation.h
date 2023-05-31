#pragma once
#ifndef __QUADRANGULATION_H__
#define __QUADRANGULATION_H__

#include "Eigen/Core"
#include <iostream>

#include <functional>
#include <tuple>

#define PI (3.1415926535897932346f)
#define recombination_treshold_ (0.10000f)

namespace quadrangulation {
    // ���ڵ��Ĳ�������һ���ṹ�壬�����Ƚ�Q-EDGE��һ���ݽṹ
    typedef struct qualityquad
    {
        double quality;
        int quad1,quad2,quad3,quad4;
        //��������Ԫ���ǽṹ��ģ����ڽṹ���ڲ�����ȽϺ��������水��id,length,width��������  
        bool operator< (const qualityquad & a)  const
        {
            return quality < a.quality;
        }
    }QualityQuad;

    // ���ڷ��ش�С�������е�edge
    template <typename type>
    std::array<type, 2> makeEdge(const type edge0, const type edge1) {
        std::array<type, 2> edge = { edge0,edge1 };
        edge = edge[1] < edge[0] ? std::array<type, 2>{edge[1], edge[0]} : edge;
        return edge;
    }

    //1. ����edges��������Ϊ�ⲿ�� n*2���ڲ��� m*2��quad k*4
    std::tuple<Eigen::ArrayXXi, Eigen::ArrayXXi> createEdges(Eigen::Ref<Eigen::ArrayXXi const> const triangulation);

    //2. �����������κϲ�Ϊһ���ı��Σ�����ı��νڵ�quad
    Eigen::ArrayXXi merge2triangles(Eigen::Ref<Eigen::ArrayXXi const> const triangulation, Eigen::Ref<Eigen::ArrayXXi const> const edge_interior);

    //3. ����ÿһ���ı��ε�����������quad�����n*1����Q�������������Խ��Խ��
    Eigen::ArrayXd element2quality(Eigen::Ref<Eigen::ArrayXXd const> const points, Eigen::Ref<Eigen::ArrayXXi const> const quadrangulation);
    
    //4. ɾ�������ߣ�����edge���ݺ�Q�������ݣ�����ɾ������С��edge��edge���1(��������delete_edge���5���ж��ܲ���ɾ��
    std::tuple<Eigen::ArrayXXi, Eigen::ArrayXXi> deleteCommonEdge(Eigen::Ref<Eigen::ArrayXXi const> const quadrangulation, Eigen::Ref<Eigen::ArrayXd const> const quality);

    //5. ������������edge
    Eigen::ArrayXXi mixEdge(Eigen::Ref<Eigen::ArrayXXi const> const edge_interior, Eigen::Ref<Eigen::ArrayXXi const> const edge_surface, Eigen::Ref<Eigen::ArrayXXi const> const edge_delete);

    //6. �ҵ�ʣ�µ�������,����ʣ���tri
    Eigen::ArrayXXi remainTri(Eigen::Ref<Eigen::ArrayXXi const> const edge_mix);

    //7. ��ʣ�µ���������������ĵ��edge���е㣬�������ηֳ������ı���,����points��quad
    std::tuple<Eigen::ArrayXXd, Eigen::ArrayXXi> refine(Eigen::Ref<Eigen::ArrayXXd const> const points, 
        Eigen::Ref<Eigen::ArrayXXi const> const new_tri, Eigen::Ref<Eigen::ArrayXXi const> const quad);
    
    //8. ��ʣ��ڵ���й⻬����ÿһ���ڵ�������ڽڵ������ƽ��ֵ�Ĳ���
    Eigen::ArrayXXd smooth(Eigen::Ref<Eigen::ArrayXXd const> const new_points, Eigen::Ref<Eigen::ArrayXXi const> const new_edge,Eigen::Ref<Eigen::ArrayXXi const> const edge_surface);
    
    //9. ������ͼ��������񼴵���edge������Ǵ��ı����������ú�����quadת��Ϊedge�ٵ���
    std::tuple< Eigen::ArrayXXd, Eigen::ArrayXXi> tri2quad(Eigen::Ref<Eigen::ArrayXXd const> const points, Eigen::Ref<Eigen::ArrayXXi const> const triangulation, bool ismix);
}

#endif