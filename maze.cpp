#include "maze.hpp"
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

Maze::Maze()
{}

Maze::Maze(const std::vector<std::vector<int> > &map):
    m_map{map}, m_path_map{map}
{}

void Maze::load_map_from_file(const std::string &path_to_file,
                              std::pair<int,int> &start,
                              std::pair<int,int> &end)
{
    std::ifstream fi(path_to_file);
    if(fi.is_open())
    {
        m_map.clear();
        int y{0};
        while(!fi.eof())
        {
            std::vector<int> row;
            std::string line;
            std::getline(fi, line);
            for(size_t x{0}; x < line.size(); ++x)
            {
                auto ch = line[x];
                if(ch == ' ' || ch == 'S' || ch == 'E')
                {
                    row.push_back(INF);
                    if(ch == 'S')
                        start = {x, y};
                    else if(ch == 'E')
                        end = {x, y};
                }
                else if(ch == 'X')
                {
                    row.push_back(-1);
                }
            }
            m_map.push_back(row);
            ++y;
        }
        m_path_map = m_map;
        m_has_map_changed = true;
        fi.close();
    }
}

void Maze::set_map(const std::vector<std::vector<int> > &map)
{
    m_map = map;
    m_has_map_changed = true;
}

void Maze::set_neighbourhood_type(Maze::NeibourhoodType nt)
{
    m_neighbourhood_type = nt;
    m_has_map_changed = true;
}

bool Maze::solve_dijkstra(std::pair<int, int> start, std::pair<int, int> end,
                          std::vector<std::pair<int, int> > &path,
                          void(*display)())
{
    if(m_has_map_changed)
    {
        m_path_map = m_map;
        const size_t height { m_map.size() };
        const size_t width { m_map[0].size() };
        bool visited[height][width];
        for(size_t i{0}; i < height; ++i)
            for(size_t j{0}; j < width; ++j)
                visited[i][j] = false;

        std::vector<std::pair<int,int> > row(width,{0,0});
        m_prev.resize(height, row);

        const int dx[] {1, 0, -1, 0, 1, -1, -1, 1};
        const int dy[] {0, 1, 0, -1, 1, 1, -1, -1};
        std::set<std::pair<int, std::pair<int,int> > > queue; // weight, row and column
        m_path_map[start.second][start.first] = 0;
        queue.insert({0, start});

        while(!queue.empty())
        {
            auto cell = *(queue.begin());
            auto curr_x = cell.second.first;
            auto curr_y = cell.second.second;
            queue.erase(queue.begin());
            visited[curr_y][curr_x] = true;
            for(int dir{0}; dir < m_neighbourhood_type; ++dir)
            {
                auto nx = curr_x + dx[dir];
                auto ny = curr_y + dy[dir];
                if(is_valid_pos({nx,ny}) &&
                   m_map[ny][nx] != WALL &&
                   !visited[ny][nx])
                {
                    if(cell.first + 1 < m_path_map[ny][nx])
                    {
                         queue.erase({m_path_map[ny][nx], {nx,ny}});
                         m_path_map[ny][nx] = cell.first + 1;
                         queue.insert({m_path_map[ny][nx], {nx,ny}});
                         m_prev[ny][nx] = cell.second;
                         display();
                    }
                }
            }
        }
    }

    if(!is_valid_pos(end) || m_path_map[end.second][end.first] == INF)
        return false;
    else
    {
        //Restore path
        path.clear();
        auto curr = end;
        while(curr != start)
        {
            path.push_back(curr);
            curr = m_prev[curr.second][curr.first];
        }
        path.push_back(curr);
        std::reverse(path.begin(), path.end());
        return true;
    }
}

bool Maze::is_valid_pos(std::pair<int,int> pos) const
{
    return !m_map.empty() && pos.second >= 0 &&
           pos.second < int(m_map.size()) &&
           pos.first >= 0 && pos.first < int(m_map[0].size());
}
