#pragma once
// ��ų�������
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <complex>
#include <limits>

namespace distmesh {
    namespace constants {
        // �������Ե��ƶ�������ֵʱ���㷨ֹͣ,pointsMovementThreshold
        static double const dptol = 1e-3;

        // �������Ե��ƶ�������ֵʱ���������ǲ���,retriangulationThreshold
        static double const ttol = 1e-1;

        // ���������е������ֵ,geometryEvaluationThreshold
        static double const geps = 1e-3;

        // ��ŷ�������µ�λ��ʱ�䲽��
        static double const deltaT = 2e-1;

        // ��ֵ΢�ֵĲ���
        // EPSILONָ���Ǹ������ɱ�ʾ����Сֵ��the smallest value��,��matlab��eps,deltaX
        static double const eps = std::sqrt(std::numeric_limits<double>::epsilon());

        // algorithm will be terminated after the maximum number of iterations,
        // when no convergence can be achieved,�㷨������������������ֹ
        static unsigned const maxSteps = 1000;
    }
}

#endif