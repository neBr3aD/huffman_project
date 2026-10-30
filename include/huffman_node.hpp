#ifndef HUFFMAN_NODE_HPP
#define HUFFMAN_NODE_HPP

#include <cstddef>

struct HuffmanNode {
    char symbol_;
    int freq_;
    std::size_t order_;
    HuffmanNode* left_;
    HuffmanNode* right_;

    HuffmanNode(
        char symbol,
        int freq,
        HuffmanNode* left = nullptr,
        HuffmanNode* right = nullptr,
        std::size_t order = 0
    );
};

HuffmanNode::HuffmanNode(
    char symbol,
    int freq,
    HuffmanNode* left,
    HuffmanNode* right,
    std::size_t order
)
    : symbol_(symbol), freq_(freq), order_(order), left_(left), right_(right) {}

struct NodeWrapper {
    HuffmanNode* node_;
    NodeWrapper(HuffmanNode* node = nullptr);
    bool operator<(const NodeWrapper& other) const;
    bool operator>(const NodeWrapper& other) const;
    bool operator==(const NodeWrapper& other) const;
    bool operator!=(const NodeWrapper& other) const;
};
NodeWrapper::NodeWrapper(HuffmanNode* node) : node_(node) {}
bool NodeWrapper::operator<(const NodeWrapper& other) const {
    if (!node_ || !other.node_) return false;
    if (node_->freq_ == other.node_->freq_) {
        return node_->order_ < other.node_->order_;
    }
    return node_->freq_ < other.node_->freq_;
}
bool NodeWrapper::operator>(const NodeWrapper& other) const {
    if (!node_ || !other.node_) return false;
    if (node_->freq_ == other.node_->freq_) {
        return node_->order_ > other.node_->order_;
    }
    return node_->freq_ > other.node_->freq_;
}
bool NodeWrapper::operator==(const NodeWrapper& other) const {
    return node_ == other.node_;
}
bool NodeWrapper::operator!=(const NodeWrapper& other) const {
    return node_ != other.node_;
}

#endif