#include <iostream>
#include <string.h>
#include <vector>
using namespace std;
static const int RULE_SIZE = 9; // current number of rules; conclusionList, ifThenList, and ifThenKey share this size
static const int VARIABLE_LIST_SIZE = 9; // current number of symptoms
static const int IF_THEN_SIZE = 5; // total size of all if/then clause variables
bool processFlag = false;
vector<string> derivedGlobalConclusionList; // vector implemented as a queue; contains validated symptoms
vector<string> derivedGlobalKeyList; // partner to global conclusion list
string clauseVarList[50]; // the clause variable list

// The conclusionList[][]: Contains all the conclusion variables.
// The order pair represents the rules "then" clause and it's associated value
// eg. [0][0], [0][1] == "Poison", "No" or [2][0], [2][1] == "Poison", "Asprin"
std::string conclusionList[12][2] = { {"Poison", "-1"}, {"Qualify", "-1"}, {"Poison", "-1"},
                                        {"Poison", "-1"}, {"Poison", "-1"}, {"Poison", "-1"},
                                        {"Poison", "-1"}, {"Poison", "-1"}, {"Poison", "-1"} };

// The variableList[][]: Contains all the symptoms and their truth value
// The order pair represents the symptom and if it is present
// -1 = uninstantiated, 0 = false, 1 = true
std::string variableList[9][2] = { {"Sick", "-1"}, {"Irregular Breathing", "-1"}, {"Cognitive Problems", "-1"}, {"Eye Problems", "-1"},
                                {"Muscle Pain", "-1"}, {"Dizzyness", "-1"}, {"Irritability", "-1"}, {"Paranoia", "-1"}, {"Sweating", "-1"} };

// The ifThenList[][]: Represents the rule list.
// Contains up to 4 symptoms per rule and conclusion assignment at the end. eg [2][4] = "Asprin"
// The conclusion assignment is only assigned when the ifThenKey's requirements are satsified  
// Otherwise the conclusion is assigned "No"                                 
std::string ifThenList[12][5] = { {"Sick", "-1", "-1", "-1", "No"}, // if not sick, then poison = no
                                    {"Sick", "-1", "-1", "-1", "Yes"}, // if sick, then qualify = yes
                                    {"Irregular Breathing", "Sweating", "-1", "-1", "Asprin"}, // 3
                                    {"Irregular Breathing", "Cognitive Problems", "Eye Problems", "-1", "Benzodiazepines"}, // 4
                                    {"Irregular Breathing", "Cognitive Problems", "Eye Problems", "-1", "Alcohol"}, // 5
                                    {"Irregular Breathing", "Cognitive Problems", "Muscle Pain", "Dizzy", "Iodine"}, // 6
                                    {"Irregular Breathing", "Cognitive Problems", "Muscle Pain", "Dizzy", "Sitmulant"}, // 7
                                    {"Irregular Breathing", "Muscle Pain", "Irritable", "-1", "Lead"}, // 8
                                    {"Irregular Breathing", "Muscle Pain", "Irritable", "-1", "Carbon Monoxide"}, // 9 
                                    {"Irregular Breathing", "Muscle Pain", "Paranoia", "-1", "Cannabis"}, // 10
                                    {"Irregular Breathing", "Muscle Pain", "Eye Problems", "-1", "Antidepressants"}, // 11 
                                    {"Irregular Breathing", "Muscle Pain", "Sweating", "-1", "Paracetamol"} }; // 12

// The ifThenKey[][]: Used to confirm requirements for if clause.
// Depending on ifThenKey matches the truth value of a symptom in the variableList
// then the correspoinding conclusionList[][] is assigned its value
// start at rule 5, irregular breathing = yes, cognitive problems = yes, eye problems = no
// rule 4 contains the correct answer how do i find the right answer given the conditions
// if there are three.. just loop until you find the answer. what condition?
// if the key confirm all but the last symptom in the ifThenList, iterate through entire ifThenList indexed at the last symptom
// 
std::string ifThenKey[12][5] = { { "0","-1","-1","-1"}, // 1
                                    { "1", "-1", "-1", "-1"}, // 2 
                                    { "1", "1", "-1", "-1"}, // 3
                                    { "1", "1", "1", "-1"}, // 4
                                    { "1", "1", "0", "-1"}, // 5
                                    { "1", "0", "1", "1"}, // 6
                                    { "1", "0", "1", "0"}, // 7
                                    { "0", "1", "1", "-1"}, // 8   
                                    { "0", "1", "0", "-1"}, // 9
                                    { "0", "0", "1", "-1"}, // 10
                                    { "0", "0", "1", "-1"}, // 11
                                    { "0", "0", "1", "-1"} }; // 12
bool processVariable = false,
     processConclusion = false;
string current_conclusion;
/********************************************************************************************* 
  ***********************************BACKWARDS CHAINING*****************************************
  **********************************************************************************************/

 // returns the conclusion variable name using variable_num
 string search_con (int variable_num) { return conclusionList[variable_num - 1][0]; }

 // converts ri into ci and returns ci
 int rule_to_clause (int ri) { return (4 * (ri - 1)) + 1; }

 // update_VLBackwards(ci): This function finds the corresponding variableList element with the clause variable list element.
 // If found, processBackwards(i) is called so that the user can verify the symptoms presence
 void update_VLBackwards (int ci);

 void processBackwards ( int variable_num );
 
 void validate_RIBackwards ( int ri, string conclusion );
 
 // test functinos

 void test_search_con (int variable_num)
 {
    current_conclusion = search_con (variable_num);
    cout << current_conclusion << endl;
 }

 int test_rule_to_clause (int ri)
 {
    int cvi = rule_to_clause(ri);
    cout << cvi << endl;
    return cvi;
 }

 void test_update_VLBackwards (int cvi)
 {
    update_VLBackwards(cvi);
    
 }

 void test_validate_RIBackwards(int ri)
 { 
    string conclusion;
    validate_RIBackwards(ri, conclusion);
 }

 bool check_if_validated(string symptom)
 {
    for (auto it = derivedGlobalConclusionList.cbegin(); it != derivedGlobalConclusionList.cend(); ++it)
        if (*it == symptom)
            return true;
    return false;
 }
int main ()
{
    int variable = 1;
	//intel->search_cvl(variable);
    // Instantiate the Clause Variable List
    // rule 1
    clauseVarList[0] = "Sick";
    clauseVarList[1] = "-1";
    clauseVarList[2] = "-1";
    clauseVarList[3] = "-1";
    // rule 2
    clauseVarList[4] = "Sick";
    clauseVarList[5] = "-1";
    clauseVarList[6] = "-1";
    clauseVarList[7] = "-1";
    // rule 3
    clauseVarList[8] = "Irregular Breathing";
    clauseVarList[9] = "-1";
    clauseVarList[10] = "-1";
    clauseVarList[11] = "-1";
    // rule 4
    clauseVarList[12] = "Irregular Breathing";
    clauseVarList[13] = "Cognitive Problems";
    clauseVarList[14] = "Eye Problems";
    clauseVarList[15] = "-1";
    // rule 5
    clauseVarList[16] = "Irregular Breathing";
    clauseVarList[17] = "Cognitive Problems";
    clauseVarList[18] = "Eye Problems";
    clauseVarList[19] = "-1";
    // rule 6
    clauseVarList[20] = "Irregular Breathing";
    clauseVarList[21] = "Cognitive Problems";
    clauseVarList[22] = "Muscle Pain";
    clauseVarList[23] = "-1";
    // rule 7
    clauseVarList[24] = "Irregular Breathing";
    clauseVarList[25] = "Cognitive Problems";
    clauseVarList[26] = "Muscle Pain";
    clauseVarList[27] = "-1";
    // rule 8
    clauseVarList[28] = "Irregular Breathing";
    clauseVarList[29] = "Muscle Pain";
    clauseVarList[30] = "-1";
    clauseVarList[31] = "-1";
    // rule 9
    clauseVarList[32] = "Irregular Breathing";
    clauseVarList[33] = "Muscle Pain";
    clauseVarList[34] = "-1";
    clauseVarList[35] = "-1";
    // rule 10
    clauseVarList[36] = "Irregular Breathing";
    clauseVarList[37] = "Muscle Pain";
    clauseVarList[38] = "-1";
    clauseVarList[39] = "-1";
    // rule 11
    clauseVarList[40] = "Irregular Breathing";
    clauseVarList[41] = "Muscle Pain";
    clauseVarList[42] = "-1";
    clauseVarList[43] = "-1";
    // rule 12
    clauseVarList[44] = "Irregular Breathing";
    clauseVarList[45] = "Muscle Pain";
    clauseVarList[46] = "-1";
    clauseVarList[47] = "-1";
    int i = 1;
    while (i < RULE_SIZE)
    {
    test_search_con(i);
    int cvi = test_rule_to_clause(i);
    test_update_VLBackwards(cvi);
    test_validate_RIBackwards(i);
    i++;
    }
    return 0;
}

// implementations
void update_VLBackwards(int ci)
  {
    // align ci to correct index
    int clause_variable_index = ci - 1;
    // look at only 4 variables in the clause variable list
    // compare each clause variable with variable list to check if variable is instantiated
    for (int i = clause_variable_index; i < clause_variable_index + 4; i++)
    {
      // if null then there are no more valid inputs
      // go to second loop
      if ( clauseVarList[i] == "-1" )
      {
        cout << "Clause Variable List has been exhausted\n";
        return;
      }
      
      // iterate through the entire variable list and find the matching variable
      for (int j = 0; j < VARIABLE_LIST_SIZE; j++)
      {
        cout << "Variable " << j << " is shown" << endl;
        // clause variable matches the variable in variableList and it is not instantiated
        if ( clauseVarList[i] == variableList[j][0] && variableList[j][1] == "-1" )
        {
          // sends the index of the variable list to process the variable
          processVariable = true;
          cout << "in processBackwards variable..." << endl;
          processBackwards(j);
          cout << "back into updateVL..." << endl;
          // break loop to find new clause variable
          break;
        }
        // if matching but not null then break to find new clause var
        else if ( clauseVarList[i] == variableList[j][0] && variableList[j][1] != "-1"){break;}
      }
    }
  
    // search through the ifThenList for matching variables
    // convert the ci back into the rule number
    // rule # = ci / 4 + 1

    int rule_number = (ci / 4) + 1;
    int rule_number_index = rule_number - 1;
    // iterate through the if variables of the ifThenList using the correspoinding rule number and the size - 1
    for (int i = 0; i < IF_THEN_SIZE - 1; i++)
      {
        // if the if variable is invalid then there are no more if variables to instantiate then break
        if (ifThenList[rule_number_index][i] == "-1")
        {
            cout << "No more valid if clauses.." << endl;
            break;
        }
        // iterate through the entire variable list to find the matching if variable
        for (int j = 0; j < VARIABLE_LIST_SIZE; j++)
          {
            cout << "Variable " << j << " is shown (processConclusion)" << endl;
            // if the if variable matches the variable in the variable list then process that variable
            if ( variableList[j][0] == ifThenList[rule_number_index][i] && variableList[j][1] == "-1")
            {
              processConclusion = true;
              cout << "in processBackwards...conclusion" << endl;
              processBackwards(j);
              cout << "back into updateVL..." << endl;
              break;
            }
            // else if the if variable matches and it has been instantiated then break
            else break;
          }
      }
  }

  // processBackwards is given the variable list number (non indexed)
   void processBackwards ( int variable_num )
 {
    /*// if both flags are false then start loop
    if ( processVariable == false && processConclusion == false )
    {
        while (true)
        {
            current_conclusion = search_con ( variable_num );
            int ri = variable_num;
            int ci = rule_to_clause ( ri );
            update_VLBackwards ( ci );
            valdiate_RIBackwards ( ri, current_conclusion );
            variable_num++;
        }
    }*/
    
    if ( processVariable == true )
    {
      // when processVariable = true then variable_num = variable_list_index
      int variable_list_index = variable_num;
      //if (variableList[variable_list_index][1] != "-1")
      string response;
      cout << "Does the patient feel/have " << variableList[variable_list_index][0] << "? (Reply Yes or No)" << endl;

      while (cin >> response)
      {
          // if yes then instantiate the variable
          if (response == "Yes" || response == "yes")
          {
            variableList[variable_list_index][1] = "1";
            processVariable = false;
            return;
          }
          // else break and return
          else if (response == "No" || response == "no")
          {
            variableList[variable_list_index][1] = "0";
            processVariable = false;
            return;
          }
          else
            cout << "Enter a correct response. (Yes or No)\n";   
      }
    }

    if ( processConclusion == true )
    {
      // when processConclusion == true then variable_num = conclusion_list_index
      int conclusion_list_index = variable_num;

      string response;
      cout << "Does the patient feel/have " << conclusionList[conclusion_list_index][0] << "? (Reply Yes or No)" << endl;

      while (cin >> response)
      {
          // if yes then instantiate the variable
          if (response == "Yes" || response == "yes")
          {
            conclusionList[conclusion_list_index][1] = "Yes";
            processConclusion = false;
            return;
          }
          // else break and return
          else if (response == "No" || response == "no")
          {
            conclusionList[conclusion_list_index][1] = "No";
            processConclusion = false;
            return;
          }
          else
            cout << "Enter a correct response. (Yes or No)\n";   
      }
    }
 }

  // check if the ifThenList variables match the ifThenKey
  // check by searching through the variable list and any instantiated global variables
  // if the key matches the variable's truth value 
  // then assign the ifThenList[rule_number_index][4] to the conclusion value and return;
  // if the key does not match the variable's truth value
  // then assign the conclusion value to "No" and continue;
  // recieves ri which is non indexed
  // PROBLEM A SECOND IRREGULAR BREATHING IS PUSHED INTO THE VECTOR
  // ************************************************************ NEED TO FINISH **************************************************************
	void validate_RIBackwards ( int ri, string conclusion )
 {
    int rule_number_index = ri - 1;
    string truth_value;
    
    // if the global list is empty then skip straight to the variable list
    if ( derivedGlobalConclusionList.empty() )
    {
      printf("Global list is empty");
      // iterate through all if clauses
      for ( int i = 0; i < 4; i++ )
      {
        
        // if -1 then no valid choices, which means test was passed
        if ( ifThenList[rule_number_index][i] == "-1")
        {
          // store the proper conclusion
          cout << "The test has passed" << endl;
          conclusionList[rule_number_index][1] = ifThenList[rule_number_index][4];
          return; // exit function
        }
        // A condition is given
        else if ( ifThenList[rule_number_index][i] != "-1" )
        {
            cout << "A condition is given" << endl;
          conclusion = ifThenList[rule_number_index][i]; // reference the condition
          truth_value = ifThenKey[rule_number_index][i]; // reference the truth_value
          // iterate through entire variable list
          for ( int j = 0; j < VARIABLE_LIST_SIZE; j++ )
          {
            cout << "variable: " << j << endl;
            // if both the conclusion and the truth match
            if (conclusion == variableList[j][0] && truth_value == variableList[j][1])
            {
                cout << "A conclusion has been confirmed and pushed into vector" << endl;
              derivedGlobalConclusionList.push_back (conclusion);
              derivedGlobalKeyList.push_back (truth_value);
              break; // break to look through if clauses
            }
            // if conclusion match but truth does not
            if (conclusion == variableList[j][0] && truth_value != variableList[j][1])
            {
                cout << "The test has failed" << endl;
              conclusionList[rule_number_index][1] = "No";
              return; // leave the function
            }

          }

        }

      }
      // if end of loop is reached, then test was passed
      conclusionList[rule_number_index][1] = ifThenList[rule_number_index][4];
      return; // exit function
    }
    // else if the global list is not empty
    else if ( !derivedGlobalConclusionList.empty() )
    {
      // iterate through all if clauses
      for ( int i = 0; i < 4; i++ )
      {
        // if -1 then no valid choices and test succesful
        if ( ifThenList[rule_number_index][i] == "-1" )
        {
          conclusionList[rule_number_index][1] = ifThenList[rule_number_index][4];
          return;
        }
        // a condition is given
        if ( ifThenList[rule_number_index][i] != "-1")
        {
          conclusion = ifThenList[rule_number_index][i];
          truth_value = ifThenKey[rule_number_index][i];
          //derivedGlobalConclusionList.begin();
          //derivedGlobalKeyList.begin();
          // go through the globals first
          // push_back adds element at the end
          // -"conclusion"-> then -"conclusion"-"conclusion2"->
          //       1                    1             2
          for ( int offset = 0; offset < derivedGlobalKeyList.size(); offset++ )
          {
            // if conclusion variables and truth values match 
            if ( conclusion == derivedGlobalConclusionList.front() 
                && truth_value == derivedGlobalKeyList.front()  )
                break; // find another clause
            if ( conclusion == derivedGlobalConclusionList.front() 
                && truth_value != derivedGlobalKeyList.front()  )
            {
              conclusionList[rule_number_index][1] = "No";
              return; //exit the function
            }

          }
          // at end of derrivedGlobal for loop then check variable list
          for ( int j = 0; j < VARIABLE_LIST_SIZE; j++ )
          {
            // if both the conclusion and the truth match
            if (conclusion == variableList[j][0] && truth_value == variableList[j][1])
            {
              derivedGlobalConclusionList.push_back (conclusion);
              derivedGlobalKeyList.push_back (truth_value);
              break; // break to look through if clauses
            }
            // if conclusion match but truth does not
            if (conclusion == variableList[j][0] && truth_value != variableList[j][1])
            {
              conclusionList[rule_number_index][1] = "No";
              return; // leave the function
            }

          }

        }

      }
      // if end of loop is reached then test succesful
      conclusionList[rule_number_index][1] = ifThenList[rule_number_index][4];
      return; // exit function 
    }

 }

