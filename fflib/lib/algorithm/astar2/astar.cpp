
#include "astar.h"
#include <iostream>
using namespace std;
using namespace ff;

astar_impl_t::astar_impl_t()
{
}

int astar_impl_t::search_path(uint32_t from_index, uint32_t to_index, vector<uint32_t>& path_)
{
    open_table_t open;

    search_node_t current;
    search_node_t neighbor_node;
    vector<map_node_t*> neighbors;

    current.set_pos_index(from_index);
    open.insert(current);
    while (false == open.empty()) {
        open.pop_first(current);
        
        if (current.get_pos_index() == to_index)
        {
            break;
        }

        m_map.get_node(current.get_pos_index())->set_closed();
        m_map.get_neighbors(current.get_pos_index(), neighbors);

        for (size_t i = 0; i < neighbors.size(); ++i)
        {
            map_node_t* neighbor_map_node = neighbors[i];
            neighbor_node.set_pos_index(neighbor_map_node->get_pos_index());
            neighbor_node.set_gval(m_map.distance(current.get_pos_index(), neighbor_map_node->get_pos_index()));
            neighbor_node.set_hval(m_map.heuristic(neighbor_map_node->get_pos_index(), to_index));

            if (true == neighbor_map_node->is_open())
            {
                if (neighbor_node.get_gval() < neighbor_map_node->get_gval())
                {
                    open.update(neighbor_map_node->get_fval(), neighbor_node);
                    neighbor_map_node->set_gval(neighbor_node.get_gval());
                    neighbor_map_node->set_hval(neighbor_node.get_hval());
                    neighbor_map_node->set_parrent(current.get_pos_index());
                }
            }
            else if (false == neighbor_map_node->is_closed())
            {
                open.insert(neighbor_node);
                neighbor_map_node->set_open();
                neighbor_map_node->set_parrent(current.get_pos_index());
            }
            else {} //! closed ignore
        }
    }

    if (current.get_pos_index() == to_index)
    {
        path_.push_back(current.get_pos_index());
        uint32_t next = m_map.get_node(current.get_pos_index())->get_parrent();
        while (next != from_index)
        {
            path_.push_back(next);
            next = m_map.get_node(next)->get_parrent();
        }
        path_.push_back(from_index);
    }
    return 0;
}
