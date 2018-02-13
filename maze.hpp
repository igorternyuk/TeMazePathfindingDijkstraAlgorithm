#ifndef MAZE_H
#define MAZE_H

#include <string>
#include <vector>

class Maze
{
public:
    enum { WALL = -1, INF = 9999 };
    enum NeibourhoodType { von_Neumann = 4, Moore = 8 };
    explicit Maze();
    explicit Maze(const std::vector<std::vector<int> > &map);
    inline auto width() const noexcept { return !m_map.empty() ? m_map[0].size(): 0; }
    inline auto height() const noexcept { return m_map.size(); }
    inline int path_map_at(int x, int y) const
    { return is_valid_pos({x,y}) ? m_path_map[y][x] : -1; }
    void load_map_from_file(const std::string &path_to_file,
                            std::pair<int, int> &start,
                            std::pair<int, int> &end);
    void set_map(const std::vector<std::vector<int> > &map);
    void set_neighbourhood_type(NeibourhoodType nt);
    inline NeibourhoodType get_neighbourhood_type() const noexcept
    { return m_neighbourhood_type; }
    bool solve_dijkstra(std::pair<int,int> start, std::pair<int,int> end,
                        std::vector<std::pair<int,int> > &path,
                        void(*display)());
private:
    std::vector<std::vector<int> > m_map;
    std::vector<std::vector<int> > m_path_map;
    std::vector<std::vector<std::pair<int,int> > > m_prev;
    NeibourhoodType m_neighbourhood_type { von_Neumann };
    bool m_has_map_changed {true};

    bool is_valid_pos(std::pair<int,int> pos) const;
};

#endif // MAZE_H
