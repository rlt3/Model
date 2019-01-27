#include "draw.hpp"
#include <vector>

using namespace glm;

struct BoundingBox {
    vec3 min;
    vec3 max;
    BoundingBox () 
        : min(vec3())
        , max(vec3())
    { }

    BoundingBox (vec3 min, vec3 max)
        : min(min)
        , max(max)
    { }
    
    bool
    contains (vec3 pos)
    {
        /* offset because everything is +-0.5 */
        pos = pos + vec3(0.5, 0.5, 0.5);
        return (all(greaterThanEqual(pos, min)) &&
                all(greaterThanEqual(max, pos)));
    }

};

struct Octant {
    std::vector<BoundingBox> region;
    Octant (BoundingBox bounds, vec3 center)
    {
        region.resize(8);
		region[0] = BoundingBox(bounds.min, center);

		region[1] = BoundingBox(vec3(center.x, bounds.min.y, bounds.min.z), 
                                vec3(bounds.max.x, center.y, center.z));

		region[2] = BoundingBox(vec3(center.x, bounds.min.y, center.z),
                                vec3(bounds.max.x, center.y, bounds.max.z));

		region[3] = BoundingBox(vec3(bounds.min.x, bounds.min.y, center.z),
                                vec3(center.x, center.y, bounds.max.z));

		region[4] = BoundingBox(vec3(bounds.min.x, center.y, bounds.min.z),
                                vec3(center.x, bounds.max.y, center.z));

		region[5] = BoundingBox(vec3(center.x, center.y, bounds.min.z),
                                vec3(bounds.max.x, bounds.max.y, center.z));

		region[6] = BoundingBox(center, bounds.max);

		region[7] = BoundingBox(vec3(bounds.min.x, center.y, center.z),
                                vec3(center.x, bounds.max.y, bounds.max.z));
    }
};

class Octree {
public:
    BoundingBox region;
    std::vector<vec3> objects;
    std::vector<Octree> children;

    Octree ()
        : region(BoundingBox())
    { }

    Octree (BoundingBox region, std::vector<vec3> &list)
        : region(region)
    {
        vec3 dimensions = region.max - region.min; 

        if (list.size() <= 8) {
            objects = list;
            return;
        }

        vec3 half = dimensions / 2.f;
        vec3 center = region.min + half;

        Octant oct(region, center);

        std::vector<std::vector<vec3>> octlist(8);
        bool contained;

        for (auto &obj : list) {
            contained = false;
            for (int i = 0; i < 8; i++) {
                if (oct.region[i].contains(obj)) {
                    octlist[i].push_back(obj);
                    contained = true;
                    break;
                }
            }
            if (!contained) {
                objects.push_back(obj);
            }
        }

        children.resize(8);
        for (int i = 0; i < 8; i++) {
            children[i] = Octree(oct.region[i], octlist[i]);
        }
    }

    void
    print ()
    {
        static int tab = 0;
        for (int i = 0; i < tab; i++)
            printf("  ");

        printf("(%f, %f, %f) -> (%f, %f, %f) | %lu\n",
                region.min.x, region.min.y, region.min.z,
                region.max.x, region.max.y, region.max.z,
                objects.size());

        tab++;
        for (unsigned i = 0; i < children.size(); i++)
            children[i].print();
        tab--;
    }

    void
    get (std::vector<vec3> &list)
    {
        list.push_back(region.max - region.min);

        for (unsigned i = 0; i < children.size(); i++)
            children[i].get(list);
    }
};
