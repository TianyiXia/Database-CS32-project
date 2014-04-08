#ifndef DATABASE_INCLUDED
#define DATABASE_INCLUDED


#include <string>
#include <vector>
#include "MultiMap.h"
#include "unordered_set"

class Database
{
public:
	enum IndexType{it_none, it_indexed};
	enum OrderingType {ot_ascending, ot_descending};

	struct FieldDescriptor
	{
		std::string name;
		IndexType index;
	};

	struct SearchCriterion
	{
		std::string fieldName;
		std::string minValue;
		std::string maxValue;
	};

	struct SortCriterion
	{
		std::string fieldName;
		OrderingType ordering;
	};

	static const int ERROR_RESULT = -1;

	Database();
	~Database();
	bool specifySchema(const std::vector<FieldDescriptor>& schema);
	bool addRow(const std::vector<std::string>& rowOfData);
	bool loadFromURL(std::string url);
	bool loadFromFile(std::string filename);
	int getNumRows() const;
	bool getRow(int rowNum, std::vector<std::string>& row) const;
	int search(const std::vector<SearchCriterion>& SearchCriteria,
			   const std::vector<SortCriterion>& SortCriteria,
			   std::vector<int>& result);
private:
	Database(const Database& other);
	Database& operator=(const Database& rhs);

	std::vector<FieldDescriptor> m_schema;
	std::vector<std::vector<std::string> > m_rows;
	std::vector<MultiMap*> m_fieldIndex;
	int m_size;

	//helper:
	void applyIntersection(std::unordered_set<int>& a,std::unordered_set<int>& b,
		std::unordered_set<int>& intersect);
	void exchange(int& x, int& y)
	{
		int t = x;
		x = y;
		y = t;
	}
	bool cmp(const std::vector<std::string>& rowA, const std::vector<std::string>& rowB,
		const std::vector<SortCriterion>& SortCriteria);//compare two row based on all SortCriteria
	int Parition(Database& db,std::vector<int>& a, int lowIndex, int highIndex, const std::vector<SortCriterion>& SortCriteria);//will always use a[0] as piviot, and will return pivot's index
	void Database::QuickSort(std::vector<int>& Array,int First,int Last,const std::vector<SortCriterion>& SortCriteria);
};


#endif