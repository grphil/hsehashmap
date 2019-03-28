#include <vector>
#include <list>
#include <stdexcept>

template<typename KeyType, typename ValueType>
class HashMapIterator {
public:
    HashMapIterator (std::vector<std::list<std::pair<const KeyType, ValueType>>>* objects = nullptr): 
        hmap(objects) {
        if (objects == nullptr) {
            return;
        }
        hsh = 0;
        while (hsh < objects->size() && hmap->operator[](hsh).empty()) {
            hsh++;
        }
        if (hsh < objects->size()) {
            obj_it = objects->operator[](hsh).begin();
        }
    }

    HashMapIterator(std::vector<std::list<std::pair<const KeyType, ValueType>>>* objects, 
                    size_t hsh_,
                    typename std::list<std::pair<const KeyType, ValueType>>::iterator obj_it_):
        hmap(objects),
        hsh(hsh_),
        obj_it(obj_it_) {}

    HashMapIterator(const HashMapIterator<KeyType, ValueType>& other_iter):
        hmap(other_iter.hmap),
        hsh(other_iter.hsh),
        obj_it(other_iter.obj_it) {}

    HashMapIterator& operator++() {
        obj_it++;
        while (hsh < hmap->size() && obj_it == hmap->operator[](hsh).end()) {
            hsh++;
            if (hsh == hmap->size()) {
                break;
            }
            obj_it = hmap->operator[](hsh).begin();
        }
        return *this;
    }

    HashMapIterator operator++(int) {
        auto it = *this;
        ++(*this);
        return it;
    }

    std::pair<const KeyType, ValueType>& operator *() {
        return *obj_it;
    }

    std::pair<const KeyType, ValueType>* operator ->() {
        return &(*obj_it);
    }

    bool operator == (const HashMapIterator<KeyType, ValueType>& a) {
        if (obj_it == a.obj_it) {
            return true;
        }
        return hmap == a.hmap && hsh == hmap->size() && a.hsh == hsh;
    }

    bool operator != (const HashMapIterator<KeyType, ValueType>& a) {
        if (obj_it == a.obj_it) {
            return false;
        }
        return hmap != a.hmap || hsh != hmap->size() || a.hsh != hsh;
    }

    std::vector<std::list<std::pair<const KeyType, ValueType>>>* hmap_ptr() {
        return hmap;
    }

    std::vector<std::list<std::pair<const KeyType, ValueType>>>* get_hmap() const {
        return hmap;
    }

    size_t get_hsh() const {
        return hsh;
    }

    typename std::list<std::pair<const KeyType, ValueType>>::iterator GetObj_it() const {
        return obj_it;
    }

private:
    std::vector<std::list<std::pair<const KeyType, ValueType>>>* hmap;
    size_t hsh;
    typename std::list<std::pair<const KeyType, ValueType>>::iterator obj_it;
};

template<typename KeyType, typename ValueType>
class HashMapConstIterator {
public:
    HashMapConstIterator (const std::vector<std::list<std::pair<const KeyType, ValueType>>>* objects = nullptr): 
        hmap(objects) {
        if (objects == nullptr) {
            return;
        }
        hsh = 0;
        while (hsh < objects->size() && hmap->operator[](hsh).empty()) {
            hsh++;
        }
        if (hsh < objects->size()) {
            obj_it = objects->operator[](hsh).begin();
        }
    }

    HashMapConstIterator(const std::vector<std::list<std::pair<const KeyType, ValueType>>>* objects, 
                         size_t hsh_, 
                         typename std::list<std::pair<const KeyType, ValueType>>::const_iterator obj_it_):
        hmap(objects),
        hsh(hsh_),
        obj_it(obj_it_) {}

    HashMapConstIterator(const HashMapIterator<KeyType, ValueType>& other_iter):
        hmap(other_iter.get_hmap()),
        hsh(other_iter.get_hsh()),
        obj_it(other_iter.GetObj_it()) {}

    HashMapConstIterator(const HashMapConstIterator<KeyType, ValueType>& other_iter):
        hmap(other_iter.hmap),
        hsh(other_iter.hsh),
        obj_it(other_iter.obj_it) {}

    HashMapConstIterator& operator++() {
        obj_it++;
        while (hsh < hmap->size() && obj_it == hmap->operator[](hsh).end()) {
            hsh++;
            if (hsh == hmap->size()) {
                break;
            }
            obj_it = hmap->operator[](hsh).begin();
        }
        return *this;
    }

    HashMapConstIterator operator++(int) {
        auto it = *this;
        ++(*this);
        return it;
    }

    const std::pair<const KeyType, ValueType>& operator *() {
        return *obj_it;
    }

    const std::pair<const KeyType, ValueType>* operator ->() {
        return &(*obj_it);
    }

    bool operator == (const HashMapConstIterator<KeyType, ValueType>& a) {
        if (obj_it == a.obj_it) {
            return true;
        }
        return hmap == a.hmap && hsh == hmap->size() && a.hsh == hsh;
    }

    bool operator != (const HashMapConstIterator<KeyType, ValueType>& a) {
        if (obj_it == a.obj_it) {
            return false;
        }
        return hmap != a.hmap || hsh != hmap->size() || a.hsh != hsh;
    }

private:
    const std::vector<std::list<std::pair<const KeyType, ValueType>>>* hmap;
    size_t hsh;
    typename std::list<std::pair<const KeyType, ValueType>>::const_iterator obj_it;
};


template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>> 
class HashMap {
public:
    HashMap():
        hasher(),
        objects(MinSize), 
        sz(0) {}

    HashMap(const Hash& hasher_):
        hasher(hasher_),
        objects(MinSize), 
        sz(0) {}

    typedef HashMapIterator<KeyType, ValueType> iterator;
    typedef HashMapConstIterator<KeyType, ValueType>const_iterator;

    template <typename Iter>
    HashMap(Iter first, Iter last): hasher(), sz(0) {
        objects.resize(MinSize);
        while (first != last) {
            AddObj(*first);
            first++;
        }
    }

    template <typename Iter>
    HashMap(Iter first, Iter last, const Hash& hasher_): hasher(hasher_), sz(0) {
        objects.resize(MinSize);
        while (first != last) {
            AddObj(*first);
            first++;
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> lst): HashMap(lst.begin(), lst.end()) {}

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> lst, const Hash& hasher_): 
        HashMap(lst.begin(), lst.end(), hasher_) {}

    HashMap(const HashMap<KeyType, ValueType, Hash>& other_map):
        HashMap(other_map.begin(), other_map.end(), hasher) {}

    const HashMap& operator = (const HashMap<KeyType, ValueType>& other) {
        objects.clear();
        objects.resize(other.objects.size());
        for (size_t i = 0; i < other.objects.size(); i++) {
            for (const auto& obj : other.objects[i]) {
                objects[i].push_back(obj);
            }
        }
        sz = other.sz;
        hasher = other.hasher;
        return other;
    }

    const size_t size() const {
        return sz;
    }

    const bool empty() const {
        return sz == 0;
    }

    const Hash hash_function() const {
        return hasher;
    }

    void insert(const std::pair<KeyType, ValueType>& pr) {
        size_t hsh = hasher(pr.first) % objects.size();
        auto it = GetObj(pr.first, hsh);
        if (it == objects[hsh].end()) {
            objects[hsh].push_back(pr);
            sz++;
            ResizeIfNeeded();
        }
    }

    void erase(const KeyType& key) {
        EraseObj(key);
    }

    void clear() {
        sz = 0;
        objects.clear();
        objects.resize(MinSize);
    }

    iterator begin() {
        return iterator(&objects);
    }

    const_iterator begin() const {
        return const_iterator(&objects);
    }

    iterator end() {
        return iterator(&objects, objects.size(), objects.back().end());
    }

    const_iterator end() const {
        return const_iterator(&objects, objects.size(), objects.back().end());
    }

    iterator find(KeyType key) {
        size_t hsh = hasher(key) % objects.size();
        auto it = GetObj(key, hsh);
        if (it != objects[hsh].end()) {
            return iterator(&objects, hsh, it);
        }
        return end();
    }

    const_iterator find(KeyType key) const {
        size_t hsh = hasher(key) % objects.size();
        auto it = GetObj(key, hsh);
        if (it != objects[hsh].end()) {
            return const_iterator(&objects, hsh, it);
        }
        return end();
    }

    ValueType& operator[] (KeyType key) {
        size_t hsh = hasher(key) % objects.size();
        auto it = GetObj(key, hsh);
        if (it == objects[hsh].end()) {
            objects[hsh].push_back(std::make_pair(key, ValueType()));
            sz++;
            ResizeIfNeeded();
            hsh = hasher(key) % objects.size();
            it = GetObj(key, hsh);
        }
        return it->second;
    }

    const ValueType& at(KeyType key) const {
        size_t hsh = hasher(key) % objects.size();
        auto it = GetObj(key, hsh);
        if (it == objects[hsh].end()) {
            throw std::out_of_range("no element with this key");
        }
        return it->second;
    }

private:
    Hash hasher;
    std::vector<std::list<std::pair<const KeyType, ValueType>>> objects;
    size_t sz;
    static constexpr size_t ResizeCoefficient = 2;
    static constexpr size_t MaxSizeCoefficient = 4;
    static constexpr size_t MinSize = 2;

    template<typename Iter = typename std::list<std::pair<const KeyType, ValueType>>::iterator>
    Iter GetObj(const KeyType& key, size_t hsh) {
        auto it = objects[hsh].begin();
        while (it != objects[hsh].end() && !(it->first == key)) {
            it++;
        }
        return it;
    }

    template<typename Iter = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator>
    Iter GetObj(const KeyType& key, size_t hsh) const {
        auto it = objects[hsh].begin();
        while (it != objects[hsh].end() && !(it->first == key)) {
            it++;
        }
        return it;
    }

    void AddObj(const std::pair<KeyType, ValueType>& pr) {
        size_t hsh = hasher(pr.first) % objects.size();
        auto it = GetObj(pr.first, hsh);
        if (it != objects[hsh].end()) {
            it->second = pr.second;
        } else {
            objects[hsh].push_back(pr);
            sz++;
            ResizeIfNeeded();
        }
    }

    void EraseObj(const KeyType& key) {
        size_t hsh = hasher(key) % objects.size();
        auto it = GetObj(key, hsh);
        if (it != objects[hsh].end()) {
            objects[hsh].erase(it);
            sz--;
            ResizeIfNeeded();
        }
    }

    void Resize(size_t newsz) {
        if (newsz < MinSize) {
            newsz = MinSize;
        }
        std::vector<std::list<std::pair<const KeyType, ValueType>>> old_objects(std::move(objects));
        objects.clear();
        objects.resize(newsz);
        for (size_t i = 0; i < old_objects.size(); i++) {
            for (const auto& object: old_objects[i]) {
                size_t hsh = hasher(object.first) % objects.size();
                objects[hsh].push_back(object);
            }
        }
    }

    void ResizeIfNeeded() {
        if (objects.size() < MinSize) {
            return;
        }
        if (objects.size() < sz) {
            Resize(sz * ResizeCoefficient);
        }
        if (objects.size() > sz * MaxSizeCoefficient) {
            Resize(sz * ResizeCoefficient);
        }
    }
};