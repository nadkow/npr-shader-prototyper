#ifndef NPRSPR_GENERALOBJECT_H
#define NPRSPR_GENERALOBJECT_H
static int globalId;

class GeneralObject {

public:

    std::string name;
    bool visible = true;
    Node node;

protected:

    int id;
};
#endif //NPRSPR_GENERALOBJECT_H
