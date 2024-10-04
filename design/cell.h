#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell() = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
    class Impl {
    public:
        virtual ~Impl() = default;

        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;

        virtual std::vector<Position> GetReferencedCells();
        virtual bool IsCacheValid() const;
        virtual void InvalidateCache();
    };

    class EmptyImpl : public Impl {
    public: 
        Value GetValue() const override;
        std::string GetText() const override;
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text);

        Value GetValue() const override;
        std::string GetText() const override;
    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string text, const SheetInterface& sheet);
        
        Value GetValue() const override;
        std::string GetText() const override;

        std::vector<Position> GetReferencedCells() override;
        bool IsCacheValid() const override;
        void InvalidateCache() override;
    private:
        std::unique_ptr<FormulaInterface> formula_;
        const SheetInterface& sheet_;
        mutable std::optional<FormulaInterface::Value> cache_;
    };

    bool HasCircularDependency(const Impl& impl) const;
    void InvalidateCache();

    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;
    std::unordered_set<Cell*> dependent_cells_;
    std::unordered_set<Cell*> referenced_cells_;
};