#pragma once
#ifndef __READTXT_H__
#define __READTXT_H__
#include <fstream>
#include"quadrangulation.h"

template <typename type>
Eigen::Array<type, Eigen::Dynamic, Eigen::Dynamic> getTemplatePoints(const std::string template_points_dir) {
    std::cout << "no such data structure" << std::endl;
    return{};
}
template < > 
Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> getTemplatePoints(const std::string template_points_dir) {
    //���ػ�->double
    std::vector<double> vec;
    std::ifstream fin(template_points_dir);
    std::string line_info, input_result;
    int rows = 0;
    if (fin)
    {
        for (int i = 0; std::getline(fin, line_info); i++, rows++) // line�в�����ÿ�еĻ��з�
        {
            std::stringstream input(line_info);
            //���������input_result�У�������Eigen::MatrixXd points��
            for (int j = 0; input >> input_result; ++j) {
                input_result.erase(0, input_result.find_first_not_of(" "));
                input_result.erase(input_result.find_last_not_of(" ") + 1);
                
                std::string::size_type size;
                vec.push_back(stod(input_result, &size));
            }
        }
    }
    else
    {
        std::cout << "no such template points file" << std::endl;
        return{};
    }
    Eigen::ArrayXXd points(rows, 2);
    for (int i = 0; i < vec.size(); i += 2) {
        points(i / 2, 0) = vec[i];
        points(i / 2, 1) = vec[i+1];
    }
    return points;
}
template < >
Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic> getTemplatePoints(const std::string template_points_dir) {
    //���ػ�->int
    std::vector<int> vec;
    std::ifstream fin(template_points_dir);
    std::string line_info, input_result;
    int rows = 0;
    if (fin)// �и��ļ�
    {
        for (int i = 0; std::getline(fin, line_info); i++, rows++) // line�в�����ÿ�еĻ��з�
        {
            std::stringstream input(line_info);
            //���������input_result�У�������Eigen::MatrixXd points��
            for (int j = 0; input >> input_result; ++j) {
                input_result.erase(0, input_result.find_first_not_of(" "));
                input_result.erase(input_result.find_last_not_of(" ") + 1);

                std::string::size_type size;
                vec.push_back(stoi(input_result, &size));
            }
        }
    }
    else    // û�и��ļ�
    {
        std::cout << "no such template points file" << std::endl;
        return{};
    }
    Eigen::ArrayXXi points(rows, 3);
    for (int i = 0; i < vec.size(); i += 3) {
        points(i / 3, 0) = vec[i];
        points(i / 3, 1) = vec[i + 1];
        points(i / 3, 2) = vec[i + 2];
    }
    return points;
}


#endif