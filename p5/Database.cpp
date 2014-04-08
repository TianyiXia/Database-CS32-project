#include "Database.h"
#include "http.h"
#include "Tokenizer.h"
#include <string>
#include <fstream>

using namespace std;

Database::Database()
{
	m_size=0;
}

Database::~Database()
{
	m_schema.clear();
	m_rows.clear();
	for(unsigned int i=0;i<m_fieldIndex.size();i++)
		delete m_fieldIndex[i];
	m_fieldIndex.clear();
}

bool Database::specifySchema(const std::vector<FieldDescriptor>& schema)
{
	//reset (a clear method)
	if(!m_schema.empty())
	{
		m_schema.clear();
		m_rows.clear();
		for(unsigned int i=0;i<m_fieldIndex.size();i++)
			delete m_fieldIndex[i];
		m_fieldIndex.clear();
		m_size=0;
	}

	//check if it has indexed field
	bool hasIndexedField=false;
	for(unsigned int i=0;i<schema.size();i++)
		if(schema[i].index==it_indexed)
			hasIndexedField=true;
	if(!hasIndexedField)
		return false;//notice nothing has been inserted to database,no need for clear

	for(unsigned int i=0;i<schema.size();i++)
	{
		m_schema.push_back(schema[i]);
		if(schema[i].index==it_indexed)
		{
			m_fieldIndex.push_back(new MultiMap());
		}
		else
			m_fieldIndex.push_back(NULL);
	}
	return true;
}

bool Database::addRow(const std::vector<std::string>& rowOfData)
{
	if(rowOfData.size()!=m_schema.size())
		return false;
	m_rows.push_back(rowOfData);
	m_size++;
	for(unsigned int i=0;i<m_fieldIndex.size();i++)
	{
		if(m_fieldIndex[i]!=NULL)
		{
			m_fieldIndex[i]->insert(rowOfData[i],m_size-1);
		}
	}
	return true;
}

bool Database::loadFromURL(std::string url)
{
	string page;
	if(!HTTP().get(url,page))
		return false;
	//chop to lines
	string delimiters="\n";
	Tokenizer t(page,delimiters);
	string w="";
	vector<string> lines;
	while(t.getNextToken(w))
		lines.push_back(w);

	//chop to unit string
	delimiters=",";
	for(unsigned int i=0;i<lines.size();i++)
	{
		Tokenizer a(lines[i],delimiters);
		 w="";
		vector<string> eachLine;
		while(a.getNextToken(w))
			eachLine.push_back(w);
		if(i==0)//schema
		{
			vector<FieldDescriptor> schema;
			for(unsigned int j=0;j<eachLine.size();j++)
			{
				if(eachLine[j][eachLine[j].size()-1]=='*')
				{
					FieldDescriptor a;
					a.index=it_indexed;
					eachLine[j].pop_back();
					a.name=eachLine[j];
					schema.push_back(a);
				}
				else
				{
					FieldDescriptor b;
					b.index=it_none;
					b.name=eachLine[j];
					schema.push_back(b);
				}
			}
			if(!specifySchema(schema))
				return false;
		}
		else if(!addRow(eachLine))
			return false;
	}
	return true;
}


bool Database::loadFromFile(std::string filename)
{
	ifstream ifs;
	ifs.open(filename.c_str(),ios::in);
	if(!ifs)
		return false;
	char line[80];
	bool bGotLine;
	bGotLine=ifs.getline(line,80);
	if(bGotLine==false)
		return false;
	bool firstLine=true;
	while (bGotLine == true)
	{
		string delimiters=",";
		Tokenizer a(line,delimiters);
		string w="";
		vector<string> eachLine;
		while(a.getNextToken(w))
			eachLine.push_back(w);
		if(firstLine)//schema
		{
			firstLine=false;
			vector<FieldDescriptor> schema;
			for(unsigned int j=0;j<eachLine.size();j++)
			{
				if(eachLine[j][eachLine[j].size()-1]=='*')
				{
					FieldDescriptor a;
					a.index=it_indexed;
					eachLine[j].pop_back();
					a.name=eachLine[j];
					schema.push_back(a);
				}
				else
				{
					FieldDescriptor b;
					b.index=it_none;
					b.name=eachLine[j];
					schema.push_back(b);
				}
			}
			if(!specifySchema(schema))
				return false;
		}
		else if(!addRow(eachLine))
			return false;
		bGotLine=ifs.getline(line,80);
	}
	ifs.close();
	return true;
}

int Database::getNumRows() const
{
	return m_rows.size();
}

bool Database::getRow(int rowNum, std::vector<std::string>& row) const//assume rowNUm start from 0
{
	if(rowNum<0 || rowNum>=m_size)
		return false;
	/*row.clear();
	for(unsigned int i=0;i<m_schema.size();i++)
	{
		row.push_back(m_rows[rowNum][i]);
	}*/
	row=m_rows[rowNum];
	return true;
}

 int Database::search(const std::vector<SearchCriterion>& SearchCriteria,
			   const std::vector<SortCriterion>& SortCriteria,
			   std::vector<int>& result)
 {
	 result.clear();//in case result has something in it already

	 if(SearchCriteria.empty())
		 return ERROR_RESULT;

	 //sortCriteriaCheck
	 for(unsigned int i=0;i<SortCriteria.size();i++)
	 {
		 bool sortCriteriaValid=false;
		 for(unsigned int k=0;k<m_schema.size();k++)//k tells me which col of rowData we use to compare
		 {
			 if(m_schema[k].name==SortCriteria[i].fieldName &&(SortCriteria[i].ordering==ot_ascending || SortCriteria[i].ordering==ot_descending ))
			 {
				 sortCriteriaValid=true;
				 break;
			 }
		 }
		 if(!sortCriteriaValid)
			 return ERROR_RESULT;
	 }


	 //apply each sort criterion
	 unordered_set<int> rowCandidate;//possible rowNum set
	 for(unsigned int i=0;i<SearchCriteria.size();i++)//traverse through different criteria
	 {
		 //SearchCriteria check
		 //1) fieldname is not in schema (contained in (2))
		 //2) field is not indexed
		 //3)lack both min & max
		 if(SearchCriteria[i].maxValue=="" && SearchCriteria[i].minValue=="")
			 return ERROR_RESULT;
		 bool isIndex=false;
		 unsigned int j;//j denote the index of field
		 for(j=0;j<m_schema.size();j++)//find the coresponding field
		 {
			 if(m_schema[j].name==SearchCriteria[i].fieldName
				 && m_schema[j].index==it_indexed)
			 {
				 isIndex=true;
				 break;
			 }
		 }
		 if(!isIndex)
			 return ERROR_RESULT;

		 unordered_set<int> anotherSet;
		 MultiMap::Iterator it;
		 it=m_fieldIndex[j]->findEqualOrSuccessor (SearchCriteria[i].minValue);
		 //should work even if min is not given also we know j must be a indexField, it is safe to dereference

		 while(it.valid() && (it.getKey()<=SearchCriteria[i].maxValue || SearchCriteria[i].maxValue==""))//work if last key has duplicate/max unspecified
		 {
			 if(i==0)//first critera add all allowed rows
			 {
				 rowCandidate.insert(it.getValue());
			 }
			 else//other criteria: build another set and find intersect with previous candidate
			 {
				 anotherSet.insert(it.getValue());
			 }
			 it.next();
		 }
		 //filter rowCandidate,start doing this from the second searchCriteria to the end
		 if(i!=0)
		 {
			 unordered_set<int> intersect;
			 applyIntersection(rowCandidate,anotherSet,intersect);
			 rowCandidate=intersect;
		 }
	 }
	 //push to result
	 for(unordered_set<int>::iterator t=rowCandidate.begin();t!=rowCandidate.end();t++)
		 result.push_back(*t);


	 //sort
	 //for simplicity assume we first compare two row by its first column in ascending order
	 QuickSort(result,0,result.size()-1,SortCriteria);
	 return result.size();//check back did not find what to return from spec
 }




 /**************************************************helper*************************************************************************/
 void Database::applyIntersection(std::unordered_set<int>& a,std::unordered_set<int>& b,
		std::unordered_set<int>& intersect)
 {
	 for(unordered_set<int>::iterator t=a.begin();t!=a.end();t++)
	 {
		 if(b.find(*t)!=b.end())//all the things in a thta can be found in b
			 intersect.insert(*t);
	 }
 }

 
 int Database::Parition(Database& db,std::vector<int>& a, int low, int high,const std::vector<SortCriterion>& SortCriteria)
 {
	 int pivotIndex=low;
	 int pivot=a[low];//pivot is a row index of m_rows
	 do
	 {
		 //whenever we try to compare a[i] and a[j]
		 //instead of cmp their direct value
		 //we cmp their iVal and jVal, which is 
		 //specified by SortCriteria
		 while(low<=high &&  cmp(db.m_rows[a[low]],db.m_rows[pivot],SortCriteria))//lowVal<=pivothVal
			 low++;
		 while (!cmp(db.m_rows[a[high]],db.m_rows[pivot],SortCriteria))  //(highVal>pivotVal)
			 high--;
		 if(low<high)
			 exchange(a[low],a[high]);
	 }
	 while(low<high);
	 exchange(a[pivotIndex],a[high]);
	 pivotIndex=high;
	 return pivotIndex;
 }

 void Database::QuickSort(vector<int>& vec,int First,int Last,const std::vector<SortCriterion>& SortCriteria)
{
  if (Last - First >= 1 )
  {
    int PivotIndex;
    PivotIndex = Parition(*this,vec,First,Last,SortCriteria);  
    QuickSort(vec,First,PivotIndex-1,SortCriteria); // left 
    QuickSort(vec,PivotIndex+1,Last,SortCriteria);  // right
  }
}

bool Database::cmp(const std::vector<std::string>& rowA,
				   const std::vector<std::string>& rowB,
		const std::vector<SortCriterion>& SortCriteria)
{
	for(unsigned int i=0;i<SortCriteria.size();i++)
	{
		for(unsigned int k=0;k<m_schema.size();k++)//k tells me which col of rowData we use to compare
		{
			if(m_schema[k].name==SortCriteria[i].fieldName)
			{
				if(rowA[k]<rowB[k])
				{
					if(SortCriteria[i].ordering==ot_ascending)
						return true;
					else
						return false;
				}
				else if(rowA[k]>rowB[k])
				{
					if(SortCriteria[i].ordering==ot_ascending)
						return false;
					else
						return true;
				}
				else
					break;
			}
		}
	}
	return true;//not enough constraint to find whose bigger,so they are equal
}