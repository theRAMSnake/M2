class MateriaConnections
{
    constructor(src)
    {
        this.src = src;
    }

    All(type)
    {
        return this.src.filter(x => x.type === type);
    }

    AllOf(A, type, B)
    {
        return this.src.filter(x => {
            let isAQualified = A === "*" || x.A === A;
            let isBQualified = B === "*" || x.B === B;
            if(isAQualified && isBQualified)
            {
                return (type === "ParentOf" && x.type === "Hierarchy") ||
                    (type === "ExtendedBy" && x.type === "Extension") ||
                    (type === "Refers" && x.type === "Reference") ||
                    (type === "RequiredBy" && x.type === "Requirement");
            }
            else
            {
                return false;
            }
        });
    }

    Has(A, type, B)
    {
        return this.AllOf(A, type, B).length != 0;
    }

    Remove(id)
    {
        this.src = this.src.filter(x => x.id !== id);
    }

    Add(newConn)
    {
        this.src.push(newConn);
    }
}

export default MateriaConnections;
