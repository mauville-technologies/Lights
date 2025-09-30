//
// Created by ozzadar on 2025-09-07.
//

#pragma once
#include "clay/clay.h"

#include <memory>
#include <vector>
using ClayComponentLayout = Clay_ElementDeclaration;

class ClayComponentBase {
public:
    virtual ~ClayComponentBase() = default;
    virtual void Render() = 0;
    virtual std::unique_ptr<ClayComponentBase> cloneBase() const = 0;
};

template <typename DerivedType>
class ClayComponent : public ClayComponentBase {
public:
    ClayComponent() = default;

    ClayComponent(const ClayComponent& other) {
        children.reserve(other.children.size());
        for (const auto& child : other.children) {
            children.push_back(child->cloneBase());
        }
    }

    template <typename... T>
    DerivedType& operator()(T&&... comps) {
        (addChild(std::forward<T>(comps)), ...);
        return static_cast<DerivedType&>(*this);
    }

    void Render() override {
        for (const auto& child : children) {
            child->Render();
        }
    }

    std::unique_ptr<ClayComponentBase> cloneBase() const override {
        return std::make_unique<DerivedType>(static_cast<const DerivedType&>(*this));
    }

private:
    void addChild(ClayComponentBase& child) { children.push_back(child.cloneBase()); }

    std::vector<std::unique_ptr<ClayComponentBase>> children;
};