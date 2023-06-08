/**
 * @file utils/nested_map.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Nested Map
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include <optional>

// 用于判断类型是否可哈希
// 模板参数 T 为待判断类型， typename = std::void_t<> 为默认情况，用于进行 SFINAE
template <typename T, typename = std::void_t<>>
struct hashable : std::false_type
{
};

// 针对可哈希类型的特化
template <typename T>
struct hashable<T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>> : std::true_type
{
};

template <typename K, typename V>
class NestedMap
{
public:
    using Map = typename std::conditional<
        hashable<K>::value,
        std::unordered_map<K, V>,
        std::map<K, V>>::type;

protected:
    Map map;
    std::shared_ptr<NestedMap<K, V>> outer;

public:
    NestedMap() = default;

    explicit NestedMap(std::shared_ptr<NestedMap<K, V>> outer) : map(), outer(std::move(outer)) {}

    /**
     * @brief 获取指定键的值，支持向外递归查找
     *
     * @param k  键
     * @param recursive  是否递归查找
     * @return std::optional<std::reference_wrapper<V>>
     */
    std::optional<std::reference_wrapper<V>> at(const K &k, bool recursive = true)
    {
        auto it = map.find(k);
        if (it != map.end())
            return it->second;
        else if (recursive && outer)
            return (*outer).at(k, recursive);
        else
            return std::nullopt;
    }

    /**
     * @brief 插入键值对
     *
     * @param k 键
     * @param v 值
     */
    void insert(const K &k, const V &v) { map.insert({k, v}); }

    /**
     * @brief 删除指定键的值，支持向外递归查找
     *
     * @param k 键
     * @param recursive 是否递归查找
     * @return true 删除成功
     * @return false 删除失败
     */
    bool erase(const K &k, bool recursive = true)
    {
        auto c = map.erase(k);
        if (recursive && outer)
            return outer->erase(k, recursive) || c;
        else
            return c;
    }

    /**
     * @brief 判断是否存在指定键，支持向外递归查找
     *
     * @param k 键
     * @param recursive 是否递归查找
     * @return true 存在指定的键
     * @return false 不存在指定的键
     */
    bool has(const K &k, bool recursive = true) const
    {
        return map.count(k) != 0 || (recursive && outer && outer->has(k));
    }

    /**
     * @brief 返回是否为根节点
     *
     * @return true 当前节点为根节点
     * @return false 当前节点不为根节点
     */
    bool isRoot() const { return !outer; }

    /**
     * @brief 返回当前节点的引用
     *
     * @return const Map& 当前节点的引用
     */
    const Map &self() const { return map; }
};

template <typename K, typename V>
using nested_map_ptr_t = std::shared_ptr<NestedMap<K, V>>;