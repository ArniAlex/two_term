#ifndef 5Z_H 
#define 5Z_H 
class priority_queue {
public:
	virtual void add_line(const char* str, int key) = 0; 
	virtual char* search_value() const = 0;
	virtual void delete_value() = 0;
	virtual priority_queue& merge(const priority_queue& first, const priority_queue& second) = 0;
	virtual ~priority_queue() noexept = default;
};

#endif //5Z_H
