#include"quadrangulation.h"
#include <string>
#include <vector>
#include <set>
#include <map>


namespace quadrangulation {
    
    //1. ����edges��������Ϊ�ⲿ�� n*2���ڲ��� m*2��quad k*4
    std::tuple<Eigen::ArrayXXi, Eigen::ArrayXXi> createEdges(Eigen::Ref<Eigen::ArrayXXi const> const triangulation) {
        Eigen::ArrayXXi combinations(triangulation.cols(), 2);
        if (triangulation.cols() == 4) {
            combinations << 0, 1, 1, 2, 2, 3, 3, 0;
        }
        else {
            //�����������(0,1),(0,2),(1,2)
            combinations << 0, 1, 0, 2, 1, 2;
        }

        //��������set���ֱ���ȫ����bars�͹�����bars
        std::set<std::array<int, 2>> uniqueEdges, commonEdges;//set���Զ�ȥ��+����
        std::array<int, 2> edge = { {0, 0} };
        for (int combination = 0; combination < combinations.rows(); ++combination)
            for (int triangle = 0; triangle < triangulation.rows(); ++triangle) {
                edge[0] = triangulation(triangle, combinations(combination, 0));
                edge[1] = triangulation(triangle, combinations(combination, 1));

                edge = edge[1] < edge[0] ? std::array<int, 2>{edge[1], edge[0]} : edge;//С�ķ�ǰ��
                if (uniqueEdges.find(edge) != uniqueEdges.end())commonEdges.insert(edge);
                // set��findʱ�临�Ӷ�ΪlogN�����������ʱ�临�Ӷ�ΪO(3*n*log(n))
                else {
                    uniqueEdges.insert(edge);
                }
            }

        // copy set to eigen array
        Eigen::ArrayXXi edge_surface(uniqueEdges.size() - commonEdges.size(), 2),
            edge_interior(commonEdges.size(), 2);
        int indexi = 0, indexj = 0;
        for (auto const& edge : uniqueEdges) {
            if (commonEdges.find(edge) != commonEdges.end()) {
                edge_interior(indexi, 0) = edge[0];
                edge_interior(indexi, 1) = edge[1];
                indexi++;
            }
            else {
                edge_surface(indexj, 0) = edge[0];
                edge_surface(indexj, 1) = edge[1];
                indexj++;
            }

        }
        return std::make_tuple(edge_interior, edge_surface);
    }

    //2. �����������κϲ�Ϊһ���ı��Σ�����ı��νڵ�quad
    Eigen::ArrayXXi merge2triangles(Eigen::Ref<Eigen::ArrayXXi const> const triangulation, Eigen::Ref<Eigen::ArrayXXi const> const edge_interior) {
        //�����������(0,1),(0,2),(1,2)
        Eigen::ArrayXXi combinations(3, 3);combinations << 0, 1, 2, 0, 2, 1, 1, 2, 0;
        //��vector��ŵ�λ������,Ȼ������
        std::vector<std::array<int, 4>> uniqueEdges;//��һ���͵ڶ�����bar�����½ڵ���������������ʣ��Ľڵ����������ĸ���triangulation������
        std::array<int, 4> edge = { {0, 0, 0, 0} };
        for (int combination = 0; combination < combinations.rows(); ++combination)
            for (int triangle = 0; triangle < triangulation.rows(); ++triangle) {
                edge[0] = triangulation(triangle, combinations(combination, 0));
                edge[1] = triangulation(triangle, combinations(combination, 1));
                edge[2] = triangulation(triangle, combinations(combination, 2));
                edge[3] = triangle;
                edge = edge[1] < edge[0] ? std::array<int, 4>{edge[1], edge[0], edge[2],edge[3]} : edge;//С�ķ�ǰ��
                uniqueEdges.push_back(edge);
            }

        std::sort(uniqueEdges.begin(), uniqueEdges.end());
        
        //��edge_interior��ͬ�е����ݷŵ�һ��
        Eigen::ArrayXXi quad(edge_interior.rows(), 4);
        int indexj = 1;
        for (int i = 0; i < edge_interior.rows(); i++)
            for (int j = indexj; j < uniqueEdges.size(); j++){
                if ((uniqueEdges[j - 1].at(0) == uniqueEdges[j].at(0) && uniqueEdges[j].at(0) == edge_interior(i, 0))
                    && (uniqueEdges[j - 1].at(1) == uniqueEdges[j].at(1) && uniqueEdges[j].at(1) == edge_interior(i, 1))) {
                    //uniqueEdges[j].at(2)��ŵ���triangulation����������������������ꡣ��
                    quad(i, 0) = uniqueEdges[j - 1].at(2);//��ڵ�1
                    quad(i, 1) = edge_interior(i, 0);//�����߽ڵ�1
                    quad(i, 2) = uniqueEdges[j].at(2);//��ڵ�2
                    quad(i, 3) = edge_interior(i, 1);//�����߽ڵ�2
                    indexj = j+1;
                    break;
                }
            }
        return quad;
        
    }

    //3. ����ÿһ���ı��ε�����������quad�����n*1����Q�������
    Eigen::ArrayXd element2quality(Eigen::Ref<Eigen::ArrayXXd const> const points, Eigen::Ref<Eigen::ArrayXXi const> const quadrangulation) {
        const int num_elements = quadrangulation.rows();
        const int num_sides = quadrangulation.cols();//4

        // angle of ideal polygon
        double poly_angle = (num_sides - 2) * PI;//2pai
        double corner_angle = poly_angle / num_sides;//2pi/4=90��

        // allocate����Q����ռ�
        Eigen::ArrayXd Q(num_elements);

        // walk over elements
        for (int i = 0; i < num_elements; i++) {
            // allocate corners������ÿ���ı��εĹսǵĿռ�
            Eigen::ArrayXd theta(num_sides); //n*1

            // calculate corners��4
            for (int j = 0; j < num_sides; j++) {
                // get edge vectors�����������ߵ�ʸ����p.row����p���У���p������
                const Eigen::Vector2d v1 = points.row(quadrangulation(i, (j + 1) % num_sides)) - points.row(quadrangulation(i, (j + 0) % num_sides));
                const Eigen::Vector2d v2 = points.row(quadrangulation(i, (j + 2) % num_sides)) - points.row(quadrangulation(i, (j + 1) % num_sides));

                // dot product ��� a��b=|a||b|cos<a,b>
                double costheta = -v1.dot(v2) / (std::sqrt(v1.dot(v1)) * std::sqrt(v2.dot(v2)));

                // fix bounds,cos<a,b>��[-1,1]
                if (costheta < -1)costheta = -1;
                if (costheta > 1)costheta = 1;

                // calcualte angle,����Ƕ�->arccos
                theta(j) = std::acos(costheta);
            }
            // concave elements
            //360�㼴Ϊ1�����༴Ϊ�ĸ�����ռ��ʮ�ȵı����ĳ˻�
            if (std::abs(theta.sum() - poly_angle) >= 1e-4) {
                Q(i) = -1.0;
            }
            else {
                Q(i) = (1.0 - (( corner_angle - theta) / corner_angle).abs()).prod();
            }
        }

        // return quality
        return Q;
    }

    //4. ɾ�������ߣ�����edge���ݺ�Q�������ݣ�����ɾ������С��edge��edge���1(��������delete_edge���5���ж��ܲ���ɾ��
    std::tuple<Eigen::ArrayXXi, Eigen::ArrayXXi> deleteCommonEdge(Eigen::Ref<Eigen::ArrayXXi const> const quadrangulation, Eigen::Ref<Eigen::ArrayXd const> const quality) {
        // ��quality��quadrangulation��һ��->double?
        std::vector<QualityQuad> vec;
        for (unsigned int i = 0; i < quadrangulation.rows(); i++) {
            QualityQuad tmp;
            tmp.quality = quality(i);
            tmp.quad1 = quadrangulation(i, 0); tmp.quad2 = quadrangulation(i, 1); tmp.quad3 = quadrangulation(i, 2); tmp.quad4 = quadrangulation(i, 3);
            vec.push_back(tmp);
        }

        // ��quality��������->����,���໹������
        std::sort(vec.begin(), vec.end(), [](QualityQuad& q1, QualityQuad& q2) ->bool
            {
                return q2.quality < q1.quality;
            });

        // ����һ��edge���飬�жϹ������Ƿ���edge�����
        // ���ڣ����ʾ����ɾ��pass�������ڣ����ʾ��ɾ�����ĸ��ߵ����½ڵ��������
        std::set<std::array<int, 2>> fixEdges,deleteEdges;//fixEdges������¼�̶��ıߣ�deleteEdge������¼ɾ���ı�
        std::set<std::array<int, 4>> quad_new;
        for (auto const &v:vec) {
            std::array<int, 2> edge = makeEdge<int>(v.quad2, v.quad4);//2��4��ŵ��ǹ�����
            // Ҫ��֤��������̫С������̫С���п���ɾ������������
            if ((fixEdges.find(edge) == fixEdges.end()) && v.quality> recombination_treshold_) {
                deleteEdges.insert(edge);
                fixEdges.insert(makeEdge<int>(v.quad1, v.quad2));
                fixEdges.insert(makeEdge<int>(v.quad2, v.quad3));
                fixEdges.insert(makeEdge<int>(v.quad3, v.quad4));
                fixEdges.insert(makeEdge<int>(v.quad4, v.quad1));
                quad_new.insert(std::array<int, 4>{v.quad1, v.quad2, v.quad3, v.quad4});
            }
            else continue;
        }
        // copy set to eigen 
        Eigen::ArrayXXi deleteEdgeArr(deleteEdges.size(), 2), quad(quad_new.size(), 4);
        int indexi = 0, indexj = 0;
        for (auto const& edgei : quad_new) {
            quad(indexi, 0) = edgei[indexi,0];
            quad(indexi, 1) = edgei[indexi, 1];
            quad(indexi, 2) = edgei[indexi, 2];
            quad(indexi, 3) = edgei[indexi, 3];
            indexi++;
        }
        for (auto const& edgej : deleteEdges) {
            deleteEdgeArr(indexj, 0) = edgej[indexj, 0];
            deleteEdgeArr(indexj, 1) = edgej[indexj, 1];
            indexj++;
        }
       /* std::cout << "delete:\n" << deleteEdgeArr << std::endl;
        std::cout << "fix:\n" << fixEdgeArr << std::endl;
        std::cout << "num_delete: " << deleteEdgeArr.rows() << "; "
            << "num_fix:" << fixEdgeArr.rows() << "; "
            << "num_all:" << vec.size() << " " << std::endl;*/
        return std::make_tuple(deleteEdgeArr, quad);
    }

    //5. �������
    Eigen::ArrayXXi mixEdge(Eigen::Ref<Eigen::ArrayXXi const> const edge_interior, Eigen::Ref<Eigen::ArrayXXi const> const edge_surface, 
        Eigen::Ref<Eigen::ArrayXXi const> const edge_delete) {    
        //��edge_delete�ŵ�set���find�����ʱ�临�Ӷ�
        std::set<std::array<int, 2>> mixEdges, deleteEdges;
        for (unsigned int i = 0; i < edge_delete.rows(); i++) {
            std::array<int, 2>edge = makeEdge<int>(edge_delete(i, 0), edge_delete(i, 1));
            deleteEdges.insert(edge);
        }
        //�½�һ��mixedge������edge_interior�ﺬ�е�edge_delete,�о�ɾ�ˣ�û�оʹ浽mixedge��
        for (unsigned int i = 0; i < edge_interior.rows(); i++) {
            std::array<int, 2>edge = makeEdge<int>(edge_interior(i, 0), edge_interior(i, 1));
            if (deleteEdges.find(edge) == deleteEdges.end())mixEdges.insert(edge);
        }
        //�ϲ�mixedge��edge_surface������mix_edge
        for (unsigned int i = 0; i < edge_surface.rows(); i++) {
            std::array<int, 2>edge = makeEdge<int>(edge_surface(i, 0), edge_surface(i, 1));
            mixEdges.insert(edge);
        }
        //copy set to eigen
        Eigen::ArrayXXi edge_mix(mixEdges.size(), 2);
        int index = 0;
        for (auto const& edge : mixEdges) {
            edge_mix(index, 0) = edge[index, 0];
            edge_mix(index, 1) = edge[index, 1];
            index++;
        }
        return edge_mix;
    }

    //6. �ҵ�ʣ�µ�������
    Eigen::ArrayXXi remainTri(Eigen::Ref<Eigen::ArrayXXi const> const edge_mix) {
        // copy eigen to set
        std::set<std::array<int, 2>> mixEdges;
        for (unsigned int i = 0; i < edge_mix.rows(); i++) {
            std::array<int, 2>edge = makeEdge<int>(edge_mix(i, 0), edge_mix(i, 1));
            mixEdges.insert(edge);
        }
        // ��set��tri����
        std::set<std::array<int, 3>> tris;
        // double pointer->˫ָ�����ж������Ƿ�ɻ�m*nlogn
        for (unsigned int i = 1; i < edge_mix.rows(); i++) {
            int left = i - 1, right = i;
            while (edge_mix(left, 0) == edge_mix(right, 0)) {
                std::array<int, 2>edge = makeEdge<int>(edge_mix(left, 1), edge_mix(right, 1));
                if (mixEdges.find(edge) != mixEdges.end()) {
                    tris.insert(std::array<int, 3>{ edge_mix(right, 0), edge[0], edge[1] });
                }
                right++;
            }
        }
        // copy set to eigen
        Eigen::ArrayXXi new_tri(tris.size(), 3);
        int index = 0;
        for (auto const& tri : tris) {
            new_tri(index, 0) = tri[index, 0];
            new_tri(index, 1) = tri[index, 1];
            new_tri(index, 2) = tri[index, 2];
            index++;
        }
        return new_tri;
    }

    //7. ��ʣ�µ���������������ĵ��edge���е㣬�������ηֳ������ı���,����points��quad
    std::tuple<Eigen::ArrayXXd, Eigen::ArrayXXi> refine(Eigen::Ref<Eigen::ArrayXXd const> const points,
        Eigen::Ref<Eigen::ArrayXXi const> const new_tri, Eigen::Ref<Eigen::ArrayXXi const> const quad) {
        //��Ų���Ľڵ㣬ÿһ����λ��������n+ptrindex
        std::vector< std::array<double, 2>> insert_points;
        // ���������ߵ��е��ÿһ�������ε����ģ�������õ�set_points�ע��ÿһ���������Ҫ+points.rows();
        std::set<std::array<int, 4>>tmp_quad; 
        std::map<std::array<int, 2>,int>common_edge;// ����һ��temp_map������¼common edge,key��edge���꣬value���е�����
        Eigen::ArrayXXi TRIcombinations(3, 2); TRIcombinations << 0, 1, 0, 2, 1, 2;// ���
        Eigen::ArrayXXi QUAcombinations(4, 2); QUAcombinations << 0, 1, 1, 2, 2, 3,3,0;// ���

        //refine TRI
        for (int i = 0,index=-1; i < new_tri.rows(); i++) {
            // �������Ľڵ�,��һ��λ�÷���������
            Eigen::ArrayXd grivaty = (points.row(new_tri(i,0))+ points.row(new_tri(i, 1))+ points.row(new_tri(i, 2)))/3;
            insert_points.push_back(std::array<double, 2>{grivaty(0), grivaty(1)});
            index++; int griindex = index+points.rows();
            // ��һ��vector��¼�����е��index����Ϊ����ֹ����㣬��������Ǳ�Ҫ��
            std::vector<int> vec;
            for (int combination = 0; combination < TRIcombinations.rows(); ++combination) {
                // �ж��Ƿ���map��ڵĻ���ֱ��ȡvalue���������¼�point
                std::array<int, 2> key = makeEdge(new_tri(i, TRIcombinations(combination, 0)), new_tri(i, TRIcombinations(combination, 1)));
                if (common_edge.find(key) != common_edge.end()) {//�����
                    vec.push_back(common_edge[key]);
                }
                else {//key����COMMON_EDGE��
                    //�����е���points��
                    Eigen::ArrayXd middle=(points.row(new_tri(i, TRIcombinations(combination, 0))) + points.row(new_tri(i, TRIcombinations(combination, 1)))) / 2;
                    insert_points.push_back(std::array<double, 2>{middle(0), middle(1)});
                    index++;
                    //���е�����������vec��
                    vec.push_back(index + points.rows());
                    //�ٽ����½ڵ���е�����������commonedge��
                    common_edge.insert(std::pair < std::array<int, 2>, int >(key, index + points.rows()));
                }                
            }
            // ÿһ���ı��ηŽ�ȥ
            tmp_quad.insert(std::array<int, 4>{griindex, vec[0], new_tri(i, 0), vec[1]});
            tmp_quad.insert(std::array<int, 4>{griindex, vec[0], new_tri(i, 1), vec[2]});
            tmp_quad.insert(std::array<int, 4>{griindex, vec[1], new_tri(i, 2), vec[2]});
        }

        //refine QUAD
        for (int i = 0, index = insert_points.size()-1; i < quad.rows(); i++) {
            // �������Ľڵ�,��һ��λ�÷���������
            Eigen::ArrayXd grivaty = (points.row(quad(i, 0)) + points.row(quad(i, 1)) + points.row(quad(i, 2))+ points.row(quad(i, 3))) / 4;
            insert_points.push_back(std::array<double, 2>{grivaty(0), grivaty(1)});
            index++; int griindex = index + points.rows();
            // ��һ��vector��¼�����е��index����Ϊ����ֹ����㣬��������Ǳ�Ҫ��
            std::vector<int> vec;
            for (int combination = 0; combination < QUAcombinations.rows(); ++combination) {
                // �ж��Ƿ���map��ڵĻ���ֱ��ȡvalue���������¼�point
                std::array<int, 2> key = makeEdge(quad(i, QUAcombinations(combination, 0)), quad(i, QUAcombinations(combination, 1)));
                if (common_edge.find(key) != common_edge.end()) {//�����
                    vec.push_back(common_edge[key]);
                }
                else {//key����COMMON_EDGE��
                    //�����е���points��
                    Eigen::ArrayXd middle = (points.row(quad(i, QUAcombinations(combination, 0))) + points.row(quad(i, QUAcombinations(combination, 1)))) / 2;
                    insert_points.push_back(std::array<double, 2>{middle(0), middle(1)});
                    index++;
                    //���е�����������vec��
                    vec.push_back(index + points.rows());
                    //�ٽ����½ڵ���е�����������commonedge��
                    common_edge.insert(std::pair < std::array<int, 2>, int >(key, index + points.rows()));
                }
            }
            // ÿһ���ı��ηŽ�ȥ
            tmp_quad.insert(std::array<int, 4>{griindex, vec[0], quad(i, 0), vec[3]});
            tmp_quad.insert(std::array<int, 4>{griindex, vec[1], quad(i, 1), vec[0]});
            tmp_quad.insert(std::array<int, 4>{griindex, vec[2], quad(i, 2), vec[1]});
            tmp_quad.insert(std::array<int, 4>{griindex, vec[3], quad(i, 3), vec[2]});
        }
        
        //��set_quad�ϲ�
        /*Eigen::ArrayXXi new_quad(quad.rows() + tmp_quad.size(), 4);
        for (unsigned i = 0; i < quad.rows(); i++) {
            new_quad.row(i) = quad.row(i);
        }*/
        Eigen::ArrayXXi new_quad(tmp_quad.size(), 4); int indexx = 0;
        for (auto const& quads : tmp_quad) {
            new_quad(indexx, 0) = quads[indexx, 0];
            new_quad(indexx, 1) = quads[indexx, 1];
            new_quad(indexx, 2) = quads[indexx, 2];
            new_quad(indexx, 3) = quads[indexx, 3];
            indexx++;
        }
        //��points��insert_points�ϲ�
        Eigen::ArrayXXd newptrs(points.rows() + insert_points.size(),2);
        for (unsigned i = 0; i < points.rows(); i++) {
            newptrs.row(i) = points.row(i);
        }
        int indexxx = 0;
        for (auto const& intptr : insert_points) {
            newptrs(indexxx + points.rows(), 0) = intptr[indexxx, 0];
            newptrs(indexxx + points.rows(), 1) = intptr[indexxx, 1];
            indexxx++;
        }
        return std::make_tuple(newptrs, new_quad);
    }

    Eigen::ArrayXXd smooth(Eigen::Ref<Eigen::ArrayXXd const> const new_points, Eigen::Ref<Eigen::ArrayXXi const> const new_edge, Eigen::Ref<Eigen::ArrayXXi const> const edge_surface) {
        // ���߽��ŵ�set��
        std::set<int> surface;
        for (unsigned i = 0; i < edge_surface.rows(); i++) {
            surface.insert(edge_surface(i, 0));
            surface.insert(edge_surface(i, 1));
        }
        // ��new edge�������������xy������������
        Eigen::ArrayXXi swap_edge = new_edge;
        swap_edge.col(0).swap(swap_edge.col(1));
        Eigen::ArrayXXi edge(new_edge.rows() * 2, 2);
        edge << new_edge,
            swap_edge;
        // copy eigen to set
        std::vector<std::array<int,2>> vec_edge(edge.rows());
        for (unsigned i = 0; i < edge.rows();i++) {
            vec_edge.push_back(std::array<int, 2>{edge(i,0),edge(i,1)});
        }
        std::sort(vec_edge.begin(), vec_edge.end());
        // ��new_points���г�ʼ��
        Eigen::ArrayXXd points = new_points;
        // ��ÿ�������ж��Ƿ�Ϊ�߽磬���ǣ���ʼ������ times=10
        int iterator = 10;
        for (unsigned times = 0; times < iterator; times++) {
            double sumx = 0, sumy = 0; int index = 0;
            for (unsigned i = 1; i < vec_edge.size(); i++) {
                if (surface.find(vec_edge[i-1].at(0)) != surface.end())continue;
                else {
                    if (vec_edge[i - 1].at(0) == vec_edge[i].at(0)) {
                        sumx += points(vec_edge[i - 1].at(1), 0);
                        sumy += points(vec_edge[i - 1].at(1), 1);
                        index++;
                        //�ӵ����һ����ʱ��
                        if (i == vec_edge.size() - 1) {
                            sumx += points(vec_edge[i - 1].at(0), 0);
                            sumy += points(vec_edge[i - 1].at(0), 1);
                            sumx += points(vec_edge[i].at(1), 0);
                            sumy += points(vec_edge[i].at(1), 1);
                            index++;
                            //����
                            points(vec_edge[i - 1].at(0), 0) = sumx / (index + 1);
                            points(vec_edge[i - 1].at(0), 1) = sumy / (index + 1);
                        }
                    }
                    else {
                        
                        //����
                        sumx += points(vec_edge[i - 1].at(0), 0);
                        sumy += points(vec_edge[i - 1].at(0), 1);
                        sumx += points(vec_edge[i - 1].at(1), 0);
                        sumy += points(vec_edge[i - 1].at(1), 1);
                        index++;

                        //����
                        points(vec_edge[i - 1].at(0), 0) = sumx / (index + 1);
                        points(vec_edge[i - 1].at(0), 1) = sumy / (index + 1);

                        //��0
                        sumx = 0; sumy = 0; index = 0;
                    }

                }
            }
        }
        // ��ÿ���ڵ��ҵ������ڽڵ����꣬�����ֵ�����µ������꼴�ɡ�
        // ���ֵ������sum��index���㣬����ʱ�临�Ӷ�

        return points;
    }

    std::tuple< Eigen::ArrayXXd,Eigen::ArrayXXi> tri2quad(Eigen::Ref<Eigen::ArrayXXd const> const points, Eigen::Ref<Eigen::ArrayXXi const> const triangulation,bool ismix) {
        Eigen::ArrayXXi edge_surface, edge_interior;
        std::tie(edge_interior, edge_surface) = quadrangulation::createEdges(triangulation);
        std::cout << "interior:\n" << edge_interior << std::endl;
        std::cout << "surface:\n" << edge_surface << std::endl;

        Eigen::ArrayXXi quad = quadrangulation::merge2triangles(triangulation, edge_interior);
        std::cout << "quad:\n" << quad << std::endl;

        Eigen::ArrayXd quality = quadrangulation::element2quality(points, quad);
        std::sort(quality.begin(), quality.end());
        std::cout << "quality:\n" << quality << std::endl;

        Eigen::ArrayXXi edge_delete, quad_delete;
        std::tie(edge_delete, quad_delete) = quadrangulation::deleteCommonEdge(quad, quality);
        std::cout << "edge_delete:\n" << edge_delete << std::endl;
        std::cout << "quad_delete:\n" << quad_delete << std::endl;

        Eigen::ArrayXXi edge_mix = quadrangulation::mixEdge(edge_interior, edge_surface, edge_delete);
        if (!ismix) {
            Eigen::ArrayXXi edge_mix = quadrangulation::mixEdge(edge_interior, edge_surface, edge_delete);

            Eigen::ArrayXXi tri_new = quadrangulation::remainTri(edge_mix);
            std::cout << "tri_new:\n" << tri_new <<tri_new.rows() << std::endl;
            //���û��ʣ�������Σ��Ǿ��Ǵ��ı�����
            if (tri_new.rows() == 0) {
                std::cout << "all elements are quadrangulation!" << std::endl;
                return std::make_tuple(points, edge_mix);
            }

            Eigen::ArrayXXd points_add; Eigen::ArrayXXi quad_new;
            std::tie(points_add, quad_new) = quadrangulation::refine(points, tri_new, quad_delete);
            std::cout << "points_add1:\n" << points_add << std::endl;
            std::cout << "quad_new:\n" << quad_new << std::endl;

            // ��quad_newת��Ϊedge
            std::set<std::array<int, 2>> edge_last_Set; 
            Eigen::ArrayXXi combinations(4, 2); combinations << 0, 1, 1, 2, 2, 3, 3, 0;
            for (int combination = 0; combination < combinations.rows(); ++combination)
                for (int quad = 0; quad < quad_new.rows(); ++quad) {
                    std::array<int, 2> edge_last_tmp = makeEdge(quad_new(quad, combinations(combination, 0)), quad_new(quad, combinations(combination, 1)));
                    edge_last_Set.insert(edge_last_tmp);
                }
            Eigen::ArrayXXi edge_last(edge_last_Set.size(),2);//copy set to eigen
            int edge_last_set_index = 0;
            for (auto const& edge_last_set : edge_last_Set) {
                edge_last(edge_last_set_index, 0) = edge_last_set[edge_last_set_index, 0];
                edge_last(edge_last_set_index, 1) = edge_last_set[edge_last_set_index, 1];
                edge_last_set_index++;
            }

            // �����ı��ε�surface_edge
            Eigen::ArrayXXi edge_surface_quad, edge_interior_quad;
            std::tie(edge_interior_quad, edge_surface_quad) = quadrangulation::createEdges(quad_new);

            Eigen::ArrayXXd points_last = quadrangulation::smooth(points_add, edge_last, edge_surface_quad);
            std::cout << "points_last:\n" << points_last << std::endl;
            return std::make_tuple(points_last, edge_last);
        }
        return std::make_tuple(points,quadrangulation::mixEdge(edge_interior, edge_surface, edge_delete));
        
    }
}