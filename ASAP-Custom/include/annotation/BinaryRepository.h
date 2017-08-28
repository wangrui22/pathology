#ifndef BINARY_REPOSITORY_H_
#define BINARY_REPOSITORY_H_

#include "annotation_export.h"
#include "Repository.h"

class ANNOTATION_EXPORT BinaryRepository : public Repository
{
public:
    BinaryRepository(const std::shared_ptr<AnnotationList>& list);
    virtual ~BinaryRepository();

    virtual bool save() const;

    void close_rsa(bool close_flag);

private:
    virtual bool loadFromRepo();
protected:
private:
    bool _rsa;
};

#endif // !BINARY_REPOSITORY_H_
