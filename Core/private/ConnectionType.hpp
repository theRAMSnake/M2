#pragma once

namespace materia
{

enum class ConnectionType
{
    //A is parent, B is child. Recursive hierarchy is not allowed. B is owned by A.
    Hierarchy,

    //B extends A. There can be only one extension of A. Non recursive. B is owned by A.
    Extension,

    //B refered by A. No restrictions.
    Reference,

    //B requires A. No loops allowed. A and B are both independent of each other.
    Requirement
};

}
