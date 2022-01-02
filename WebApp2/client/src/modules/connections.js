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

    Has(A, type, B)
    {
        return this.src.filter(x => {
            let isAQualified = A === "*" || x.A === A;
            let isBQualified = B === "*" || x.B === B;
            if(isAQualified && isBQualified)
            {
                return (type === "ParentOf" && x.type === "Hierarchy") ||
                    (type === "ExtendedBy" && x.type === "Extension") ||
                    (type === "ReferedBy" && x.type === "Reference") ||
                    (type === "RequiredBy" && x.type === "Requirement");
            }
            else
            {
                return false;
            }
        }).length != 0;
    }
}

export default MateriaConnections;
