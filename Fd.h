#pragma once
#ifndef __FD_H__
#define __FD_H__

#include "functional.h"

namespace distmesh {
    namespace Fd {
        // creates distance function for a nd rectangular domain,����������ľ��뺯��
        // Attention: Not a real distance function at the corners of domainm,ע�⣺��Ľǵ㴦�ľ��뺯������ʵ��
        // you have to give the corners as fixed points to distmesh algorithm,���뽫�ǵ���Ϊ�̶����ṩ��DistMesh�㷨
        Functional rectangular(Eigen::Ref<Eigen::ArrayXXd const> const rectangle);

        // creates the true distance function for a 2d rectangular domain,����
        Functional rectangle(Eigen::Ref<Eigen::ArrayXXd const> const rectangle);

        // creates distance function for elliptical domains��������Բ��ľ��뺯��
        // Note: not a real distance function but a level function,ע�⣺����ʵ�����뺯��������ˮƽ����
        // which is sufficient
        Functional elliptical(Eigen::Ref<Eigen::ArrayXd const> const radii = Eigen::ArrayXd(),
            Eigen::Ref<Eigen::ArrayXd const> const midpoint = Eigen::ArrayXd());

        // creates the true distance function for circular domains��Բ
        Functional circular(double const radius = 1.0,
            Eigen::Ref<Eigen::ArrayXd const> const midpoint = Eigen::ArrayXd());

        // creates distance function for a 2d domain described by polygon
        // Attention: Not a real distance function at the corners of domainm
        // you have to give the corners as fixed points to distmesh algorithm�������
        Functional polygon(Eigen::Ref<Eigen::ArrayXXd const> const polygon);
    }
}

#endif