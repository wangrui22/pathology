#include "AnnotationList.h"
#include "Annotation.h"
#include "AnnotationGroup.h"
#include "Annotation_define.h"

AnnotationList::AnnotationList() {
}

AnnotationList::~AnnotationList() {
  removeAllAnnotations();
  removeAllGroups();
}

bool AnnotationList::isModified() {
  for (std::vector<std::shared_ptr<Annotation> >::iterator it = _annotations.begin(); it != _annotations.end(); ++it) {
    if ((*it)->isModified()) {
      return true;
    }
  }
  for (std::vector<std::shared_ptr<AnnotationGroup> >::iterator it = _groups.begin(); it != _groups.end(); ++it) {
    if ((*it)->isModified()) {
      return true;
    }
  }
  return false;
}

void AnnotationList::resetModifiedStatus() {
  for (std::vector<std::shared_ptr<Annotation> >::iterator it = _annotations.begin(); it != _annotations.end(); ++it) {
    (*it)->resetModifiedStatus();
  }
  for (std::vector<std::shared_ptr<AnnotationGroup> >::iterator it = _groups.begin(); it != _groups.end(); ++it) {
    (*it)->resetModifiedStatus();
  }
}

bool AnnotationList::addGroup(const std::shared_ptr<AnnotationGroup>& group) {
  if (group) {
    _groups.push_back(group);
    return true;
  }
  return false;
}

bool AnnotationList::addAnnotation(const std::shared_ptr<Annotation>& annotation) {
  if (annotation) {
    _annotations.push_back(annotation);
    return true;
  }
  return false;
}

std::shared_ptr<AnnotationGroup> AnnotationList::getGroup(const int& index) {
  if (index < 0) {
    return *(_groups.end() - abs(index));
  }
  else {
    return *(_groups.begin() + index);
  }
}

std::shared_ptr<AnnotationGroup> AnnotationList::getGroup(std::string name) {
  for (std::vector<std::shared_ptr<AnnotationGroup> >::const_iterator it = _groups.begin(); it != _groups.end(); ++it) {
    if ((*it) && (*it)->getName() == name) {
      return (*it);
    }
  }
  return NULL;
}

std::shared_ptr<Annotation> AnnotationList::getAnnotation(const int& index) {
  if (index < 0) {
    return *(_annotations.end() - abs(index));
  }
  else {
    return *(_annotations.begin() + index);
  }
}

std::shared_ptr<Annotation> AnnotationList::getAnnotation(std::string name) {
  for (std::vector<std::shared_ptr<Annotation> >::const_iterator it = _annotations.begin(); it != _annotations.end(); ++it) {
    if ((*it) && (*it)->getName() == name) {
      return (*it);
    }
  }
  return NULL;
}

std::vector<std::shared_ptr<Annotation> > AnnotationList::getAnnotations() const {
  return _annotations;
}

std::vector<std::shared_ptr<AnnotationGroup> > AnnotationList::getGroups() const {
  return _groups;
}

void AnnotationList::setAnnotations(const std::vector<std::shared_ptr<Annotation> >& annotations) {
  this->removeAllAnnotations();
  _annotations = annotations;
}

void AnnotationList::setGroups(const std::vector<std::shared_ptr<AnnotationGroup> >& groups) {
  this->removeAllGroups();
  _groups = groups;
}

void AnnotationList::removeGroup(const int& index) {
  if (index < 0) {
    (_groups.end() - abs(index))->reset();
    _groups.erase(_groups.end() - abs(index));
  }
  else {
    (_groups.begin() + index)->reset();
    _groups.erase(_groups.begin() + index);
  }
}

void AnnotationList::removeGroup(std::string name) {
  for (std::vector<std::shared_ptr<AnnotationGroup> >::iterator it = _groups.begin(); it != _groups.end(); ++it) {
    if ((*it) && (*it)->getName() == name) {
      _groups.erase(it);
      break;
    }
  }
}

void AnnotationList::removeAnnotation(const int& index) {
  if (index < 0) {
    (_annotations.end() - abs(index))->reset();
    _annotations.erase(_annotations.end() - abs(index));
  }
  else {
    (_annotations.begin() + index)->reset();
    _annotations.erase(_annotations.begin() + index);
  }
}

void AnnotationList::removeAnnotation(std::string name) {
  for (std::vector<std::shared_ptr<Annotation> >::iterator it = _annotations.begin(); it != _annotations.end(); ++it) {
    if ((*it) && (*it)->getName() == name) {
      _annotations.erase(it);
      break;
    }
  }
}

void AnnotationList::removeAllAnnotations() {
  _annotations.clear();
}

void AnnotationList::removeAllGroups() {
  _groups.clear();
}

//////////////////////////////////////////////////////////////////////////
//Binary buffer , use .araw format but two different : 
    //1 don't entrypt entrypt_tumor_type_id
    //2 don't entrypt coordinates 
//Filer header(struct)
//Groups(struct)
//Annotations(struct)
//All coordinates(float array)
bool AnnotationList::read(char* input_buffer, unsigned int size)
{
    _groups.clear();
    _annotations.clear();

    char* buffer = input_buffer;
    if (nullptr == buffer)
    {
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //1 Read file header
    //Check size
    if (size < sizeof(AnnotationFileHeader))//FILE CRASH
    {
        return false;
    }

    AnnotationFileHeader* file_header = (AnnotationFileHeader*)buffer;
    if (!file_header->valid)//FILE CRASH
    {
        return false;
    }

    if (file_header->valid && file_header->anno_num == 0 && file_header->anno_num == 0)//FILE Valid but empty annotations
    {
        return true;
    }

    const int group_num = file_header->group_num;
    const int anno_num = file_header->anno_num;
    if (group_num < 0 || anno_num < 0)//Parameter invalid
    {
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //2 Read group
    buffer += sizeof(AnnotationFileHeader);
    //Check size
    if (size < sizeof(AnnotationFileHeader) + sizeof(GroupUnit)*group_num)
    {
        return false;
    }
    std::map<std::string, std::pair<std::string, std::shared_ptr<AnnotationGroup>>> group_set;
    for (int i = 0; i < group_num; ++i)
    {
        GroupUnit* group_unit = (GroupUnit*)(buffer);

        std::string name;
        char_array_to_string(group_unit->name_str, sizeof(group_unit->name_str), name);

        std::string group_name;
        char_array_to_string(group_unit->group_name_str, sizeof(group_unit->group_name_str), group_name);

        std::string color;
        char_array_to_string(group_unit->color_str, sizeof(group_unit->color_str), color);

        std::shared_ptr<AnnotationGroup> group(new AnnotationGroup());
        group->setName(name);
        group->setColor(color);

        group_set[name] = std::make_pair(group_name, group);

        this->addGroup(group);

        buffer += sizeof(GroupUnit);
    }

    //loop again to set group to group
    for (auto it = group_set.begin(); it != group_set.end(); ++it)
    {
        std::string group_name = it->second.first;
        if (!group_name.empty())
        {
            auto group = this->getGroup(group_name);
            if (group)
            {
                it->second.second->setGroup(group);
            }
            else
            {
                this->removeAllAnnotations();
                this->removeAllGroups();
                return false;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //3 Read annotation
    //Check size
    if (size < sizeof(AnnotationFileHeader) + sizeof(GroupUnit)*group_num + sizeof(AnnotationUnit)*anno_num)
    {
        this->removeAllAnnotations();
        this->removeAllGroups();
        return false;
    }

    std::vector<unsigned int> each_anno_points_num;
    unsigned int annos_points_sum = 0;
    for (int i = 0; i < anno_num; ++i)
    {
        AnnotationUnit* anno_unit = (AnnotationUnit*)buffer;

        std::string name;
        char_array_to_string(anno_unit->name_str, sizeof(anno_unit->name_str), name);

        std::string group_name;
        char_array_to_string(anno_unit->group_name_str, sizeof(anno_unit->group_name_str), group_name);

        std::string color;
        char_array_to_string(anno_unit->color_str, sizeof(anno_unit->color_str), color);

        std::shared_ptr<Annotation> anno(new Annotation());
        anno->setName(name);
        anno->setColor(color);

        if (!group_name.empty())
        {
            auto group = this->getGroup(group_name);
            if (group)
            {
                anno->setGroup(group);
            }
            else
            {
                this->removeAllAnnotations();
                this->removeAllGroups();
                return false;
            }
        }

        anno->setType(static_cast<Annotation::Type>(anno_unit->anno_type_id));
        each_anno_points_num.push_back(anno_unit->point_num);
        annos_points_sum += anno_unit->point_num;


        //Here don't need decrypt
        for (int i = 0; i < TUMOR_TYPE_NUM; ++i)
        {
            if (0 != anno_unit->entrypt_tumor_type_id[i])
            {
                anno->addTumorType(TUMOR_TYPES[i]);
            }
        }

        this->addAnnotation(anno);

        buffer += sizeof(AnnotationUnit);
    }

    //////////////////////////////////////////////////////////////////////////
    //4 read coordinate
    //Check size
    if (size < sizeof(AnnotationFileHeader) + sizeof(GroupUnit)*group_num + sizeof(AnnotationUnit)*anno_num + annos_points_sum*sizeof(float)*2)
    {
        this->removeAllAnnotations();
        this->removeAllGroups();
        return false;
    }

    std::vector<std::shared_ptr<Annotation>> anno_set = this->getAnnotations();
    for (int i=0 ; i != each_anno_points_num.size() ; ++i)
    {
        float* points_array = (float*)buffer;
        for (unsigned int j = 0 ; j<each_anno_points_num[i]; ++j)
        {
            anno_set[i]->addCoordinate(Point(points_array[j* 2], points_array[j * 2 + 1]));
        }
        buffer += each_anno_points_num[i] * sizeof(float) * 2;
    }

    assert((buffer - input_buffer) == size);

    return true;
}

//////////////////////////////////////////////////////////////////////////
//Binary buffer , use .araw format but two different : 
//1 don't entrypt entrypt_tumor_type_id
//2 don't entrypt coordinates 
//Filer header(struct)
//Groups(struct)
//Annotations(struct)
//All coordinates(float array)
bool AnnotationList::write(char*& out_buffer, unsigned int& size)
{
    const std::vector<std::shared_ptr<Annotation> >& annos = this->getAnnotations();
    const std::vector<std::shared_ptr<AnnotationGroup> >& groups = this->getGroups();

    const int group_num = static_cast<int>(groups.size());
    const int anno_num = static_cast<int>(annos.size());

    unsigned int point_num(0);
    for (int i = 0; i < annos.size(); ++i)
    {
        point_num += annos[i]->getNumberOfPoints();
    }

    size = sizeof(AnnotationFileHeader) + group_num * sizeof(GroupUnit) + anno_num * sizeof(AnnotationUnit) + point_num * 2 * sizeof(float);
    out_buffer = new char[size];

    char* buffer = out_buffer;

    //////////////////////////////////////////////////////////////////////////
    //1 write header
    AnnotationFileHeader* header = (AnnotationFileHeader*)buffer;
    header->group_num = group_num;
    header->anno_num = anno_num;
    header->valid = 0;//set to 1 in the end
    buffer += sizeof(AnnotationFileHeader);

    //////////////////////////////////////////////////////////////////////////
    //2 write groups info
    for (int i = 0; i < group_num; ++i)
    {
        GroupUnit *group = (GroupUnit*)buffer;

        std::string name = groups[i]->getName();
        if (name.size() > 255)
        {
            delete[] buffer;
            size = 0;
            return false;
        }

        std::string group_name = "";
        if (groups[i]->getGroup())
        {
            group_name = groups[i]->getGroup()->getName();
        }
        if (group_name.size() > 255)
        {
            delete[] buffer;
            size = 0;
            return false;
        }

        std::string color = groups[i]->getColor();

        memcpy(group->name_str, name.c_str(), name.size());
        memcpy(group->group_name_str, group_name.c_str(), group_name.size());
        memcpy(group->color_str, color.c_str(), color.size());

        group->name_str[name.size()] = '\0';
        group->group_name_str[group_name.size()] = '\0';
        group->color_str[color.size()] = '\0';

        buffer += sizeof(GroupUnit);
    }

    //////////////////////////////////////////////////////////////////////////
    //3 write annotation
    for (int i = 0; i < anno_num; ++i)
    {
        AnnotationUnit *unit = (AnnotationUnit*)buffer;

        std::string name = annos[i]->getName();
        if (name.size() > 255)
        {
            delete[] buffer;
            size = 0;
            return false;
        }

        std::string group_name = "";
        if (annos[i]->getGroup())
        {
            group_name = annos[i]->getGroup()->getName();
        }
        if (group_name.size() > 255)
        {
            delete[] buffer;
            size = 0;
            return false;
        }

        std::string color = annos[i]->getColor();

        memcpy(unit->name_str, name.c_str(), name.size());
        memcpy(unit->group_name_str, group_name.c_str(), group_name.size());
        memcpy(unit->color_str, color.c_str(), color.size());

        unit->name_str[name.size()] = '\0';
        unit->group_name_str[group_name.size()] = '\0';
        unit->color_str[color.size()] = '\0';

        unit->point_num = annos[i]->getNumberOfPoints();

        //tumor types (no need entrypt)
        unit->anno_type_id = static_cast<int>(annos[i]->getType());
        std::set<std::string> tumor_types = annos[i]->getTumorTypes();

        memset(unit->entrypt_tumor_type_id, 0, sizeof(unit->entrypt_tumor_type_id));
        for (auto it = tumor_types.begin(); it != tumor_types.end(); ++it)
        {
            int id = get_tumor_type_id(*it);
            if (-1 != id)
            {
                unit->entrypt_tumor_type_id[id] = 1;
            }
        }

        buffer += sizeof(AnnotationUnit);
    }

    //////////////////////////////////////////////////////////////////////////
    //4 write coordinate
    std::vector<std::shared_ptr<Annotation>> anno_set = this->getAnnotations();
    for (int i = 0; i < anno_set.size(); ++i)
    {
        std::vector<Point> points = anno_set[i]->getCoordinates();
        float *point_array = (float*)(buffer);
        for (int j = 0 ; j< points.size() ; ++j)
        {
            point_array[j * 2] = points[j].getX();
            point_array[j * 2+1] = points[j].getY();
        }

        buffer += sizeof(float) * 2 * points.size();
    }
    assert( (buffer - out_buffer) == size);

    //////////////////////////////////////////////////////////////////////////
    //5 Write valid tag
    header->valid = 1;//set to 1 in the end
    return true;
}
