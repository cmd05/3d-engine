#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Types.hpp>

// SceneViewExclude

template<typename ...ExcludeTypes>
struct SceneViewExclude {};

// SceneView

template<typename ...ComponentTypes>
class SceneView {
public:
    class iterator;
    
    template<typename ...ExcludeTypes>
    SceneView(Scene& scene, SceneViewExclude<ExcludeTypes...> exclude);
    
    SceneView(Scene& scene, bool exclusive = false);

    iterator begin() const { return m_begin; }
    iterator end() const { return m_end; }

private:
    Scene* const m_scene;

    bool m_exclusive;
    Signature m_excluded; // default std::bitset is all zero's

    iterator m_begin;
    iterator m_end;
};

/// SceneView Iterator

template<typename ...ComponentTypes>
class SceneView<ComponentTypes...>::iterator {
public:
    iterator() {}
    iterator(vector_entity_iterator it, SceneView* scene): vec_iterator{it}, scene_view{scene} {}

    iterator& operator++();
    
    bool is_valid_entity(const iterator& it) const;
    
    bool operator==(const iterator& other) const { return !operator!=(other);  }
    
    bool operator!=(const iterator& other) const;

    const Entity& operator*() const { return *vec_iterator; }

private:
    vector_entity_iterator vec_iterator;
    SceneView* scene_view;
    mutable bool first_check = true;
};

template<typename ...ComponentTypes>
bool SceneView<ComponentTypes...>::iterator::operator!=(const iterator& other) const {
    if(first_check) {
        first_check = false;
    
        // check that vector iterators are unequal (equal when this->vec_iterator == end()),
        // before calling is_valid_entity with `*this`
        return (vec_iterator != other.vec_iterator) && is_valid_entity(*this); 
    } else {
        return (vec_iterator != other.vec_iterator);
    }
}

template<typename ...ComponentTypes>
bool SceneView<ComponentTypes...>::iterator::is_valid_entity(const iterator& it) const {
    Signature signature_required = scene_view->m_scene->get_components_signature<ComponentTypes...>();
    Signature signature_entity = scene_view->m_scene->get_entity_signature(*it.vec_iterator);

    return ( 
        (!scene_view->m_exclusive &&
            (signature_entity & scene_view->m_excluded).none() &&
            (signature_required & signature_entity) == signature_required) ||
        (scene_view->m_exclusive && signature_required == signature_entity)
    );
}

template<typename ...ComponentTypes>
typename SceneView<ComponentTypes...>::iterator& SceneView<ComponentTypes...>::iterator::operator++() {
    do
        vec_iterator++;
    while(*this != scene_view->end() && !is_valid_entity(*this));

    return *this;
}

// SceneView Methods

template<typename ...ComponentTypes>
template<typename ...ExcludeTypes>
SceneView<ComponentTypes...>::SceneView(Scene& scene, SceneViewExclude<ExcludeTypes...> exclude): SceneView(scene) {
    m_excluded = m_scene->get_components_signature<ExcludeTypes...>();
}

template<typename ...ComponentTypes>
SceneView<ComponentTypes...>::SceneView(Scene& scene, bool exclusive): m_scene{&scene} {
    m_exclusive = exclusive;

    auto [begin, end] = m_scene->get_smallest_component_array<ComponentTypes...>();

    m_begin = iterator{begin, this};
    m_end = iterator{end, this};
}