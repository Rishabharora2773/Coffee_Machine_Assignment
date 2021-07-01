#include<iostream>
#include<map>
#include<fstream>
using namespace std;
const int MIN_INGREDIENT_THRESHOLD = 10;
ifstream fin("input.txt");      // file reader

class BeverageRecipe {
    private:
        string beverageName;
        map<string,int> recipe;

    public:
        BeverageRecipe(string _beverageName, map<string,int> _recipe) {
            this->beverageName = _beverageName;
            this->recipe = _recipe;
        }

        string getBeverageName() {
            return this->beverageName;
        }

        map<string,int> getRecipeMap() {
            return this->recipe;
        }
};

class CoffeeMachine {
    private:
        int numOfOutlets;
        map<string, BeverageRecipe* > beverageMap;
        map<string,int> availableIngredientsMap;


    public:
        // Initializing Coffee Machine with number of outlets
        CoffeeMachine(int _numOfOutlets) {
            this->numOfOutlets = _numOfOutlets;
            initializeIngredients();
            initializeRecipes();
        }
        

        // Initialize all ingredients by reading from input file
        void initializeIngredients() {
            string line;
            getline(fin,line,'\n'); // empty line
            getline(fin,line,'\n');

            while(1) {
                getline(fin,line,'\n');
                if(line.size()>0) {
                    int pos = line.find(":");
                    string ingredient = line.substr(0,pos);
                    int quantity = stoi(line.substr(pos+2));
                    this->availableIngredientsMap[ingredient] = quantity;
                } else break;
            }
        }


        // Initialize all recipes by reading from input file
        void initializeRecipes() {
            string line;
            while(1) {
                getline(fin,line);
                if(fin.eof() || line=="") break;
                string beverageName = line.substr(0,line.size()-1);

                map<string,int> recipeMap;
                while(line!="") {
                    getline(fin,line);
                    if(line=="") break;

                    int pos = line.find(":");
                    string ingredient = line.substr(4,pos-4);
                    int quantity = stoi(line.substr(pos+2));
                    recipeMap[ingredient] = quantity;
                }

                BeverageRecipe* beverageRecipe = new BeverageRecipe(beverageName, recipeMap);
                this->beverageMap[beverageName] = beverageRecipe;
            }
        }


        // Check if a beverage can be prepared with available quantities
        bool canPrepareBeverage(string beverageName) {
            BeverageRecipe* beverageRecipe = beverageMap[beverageName];
            map<string,int> beverageRecipeMap = beverageRecipe->getRecipeMap();

            map<string,int> :: iterator recipeIterator;

            // update available quantity for every ingredient in the recipe
            for(recipeIterator=beverageRecipeMap.begin();recipeIterator!=beverageRecipeMap.end();recipeIterator++) {
                string ingredient = (*recipeIterator).first;
                int requiredQuantity = (*recipeIterator).second;

                int availableQuantity;
                bool notAvailable = false;

                if(this->availableIngredientsMap.find(ingredient) == this->availableIngredientsMap.end()) {
                    availableQuantity = 0;
                    notAvailable = true;
                } else {
                    availableQuantity = this->availableIngredientsMap[ingredient];
                }
                
                if(availableQuantity == 0 && notAvailable) {
                    cout<<beverageName<<" cannot be prepared beacause "<<ingredient<<" is not available"<<endl;
                    return false;
                }

                if(availableQuantity < requiredQuantity) {
                    cout<<beverageName<<" cannot be prepared beacause "<<ingredient<<" is not sufficient"<<endl;
                    return false;
                }
            }

            return true;
        }


        // prepare a beverage
        // 1. Check if beverage can be prepared ?
        // 2. If yes, then prepare.
        // 3. Prepare by updating their available quantites to new quantities
        void prepareBeverage(string beverageName) {
            if(this->beverageMap.find(beverageName) == this->beverageMap.end()) {
                cout<<"No recipe exists for this beverage: "<<beverageName<<endl;
                return;
            }

            if(this->canPrepareBeverage(beverageName)) {
                BeverageRecipe* beverageRecipe = beverageMap[beverageName];
                map<string,int> beverageRecipeMap = beverageRecipe->getRecipeMap();

                map<string,int> :: iterator recipeIterator;

                // update available quantity for every ingredient in the recipe
                for(recipeIterator=beverageRecipeMap.begin();recipeIterator!=beverageRecipeMap.end();recipeIterator++) {
                    string ingredient = (*recipeIterator).first;
                    int requiredQuantity = (*recipeIterator).second;

                    int availableQuantity = this->availableIngredientsMap[ingredient];

                    this->availableIngredientsMap[ingredient] = availableQuantity - requiredQuantity;

                    checkIfIngredientRunningLow(ingredient, availableQuantity - requiredQuantity);
                }

                cout<<beverageName<<" is prepared!"<<endl;
            }
        }

        // checking if the ingredient is running low
        // simply checking if the current quantity of that ingredient is less than defined threshold
        void checkIfIngredientRunningLow(string ingredient, int current_quantity) {
            if(current_quantity < MIN_INGREDIENT_THRESHOLD) {
                cout<<"Ingredient "<<ingredient<<" is running low! Please refill."<<endl;
            }
        }

        // Refill a particular ingredient with quantity
        void refill(string ingredient, int quantity) {
            if(quantity <= 0)  {
                cout<<"Refill possible only with positive amount. Please enter positive quantity!"<<endl;
                return;
            }

            if(this->availableIngredientsMap.find(ingredient) == this->availableIngredientsMap.end()) {
                this->availableIngredientsMap[ingredient] = quantity;
            } else{
                this->availableIngredientsMap[ingredient] += quantity;
            }
            cout<<"Ingredient "<<ingredient<<" refilled, current quantity: "<<this->availableIngredientsMap[ingredient]<<endl;
        }

        // add missing ingredient in the available ingredient list
        void addMissingIngredient(string ingredient, int quantity) {
            // Check if ingredient is already there
            if(this->availableIngredientsMap.find(ingredient) != this->availableIngredientsMap.end()) {
                cout<<ingredient<<" already present in the available ingredient list! Try Refilling Instead."<<endl;
                return;
            }

            // If not present, add it into the available list
            this->availableIngredientsMap[ingredient] = quantity;
        }
};

int main () {
    if(!fin) {
        cout<<"Failed to open the input file, or file input.txt doesn't exists!";
        return 0;
    }
    int numOfOutlets;
    fin >> numOfOutlets;
    CoffeeMachine coffeeMachine(numOfOutlets);
    
    /*
        Following are the functional test cases.
        You can simply uncomment them one by one & test them by checking the output in the console.
    */
    // Test Case 1: 
    //   -- 1 Ingredient is not available, and 1 not sufficient
    // coffeeMachine.prepareBeverage("hot_tea");
    // coffeeMachine.prepareBeverage("hot_coffee");
    // coffeeMachine.prepareBeverage("green_tea");
    // coffeeMachine.prepareBeverage("black_tea");
    // cout<<"\n********************************\n";

    // Test Case 2
    //   --  Adding the missing ingredient from above case, and 1 not sufficient
    // coffeeMachine.addMissingIngredient("green_mixture", 40);
    // coffeeMachine.prepareBeverage("hot_tea");
    // coffeeMachine.prepareBeverage("black_tea");
    // coffeeMachine.prepareBeverage("green_tea");
    // coffeeMachine.prepareBeverage("hot_coffee");
    // cout<<"\n********************************\n";

    // Test Case 3
    //   -- Adding the missing ingredient & refilling the insufficient one
    // coffeeMachine.addMissingIngredient("green_mixture", 40);
    // coffeeMachine.prepareBeverage("hot_tea");
    // coffeeMachine.prepareBeverage("black_tea");
    // coffeeMachine.refill("hot_water", 200);
    // coffeeMachine.prepareBeverage("green_tea");
    // coffeeMachine.prepareBeverage("hot_coffee");
    // cout<<"\n********************************\n";

    // Test Case 4
    //   -- All can be prepared, after refilling & adding missing ingredient
    // coffeeMachine.addMissingIngredient("green_mixture", 40);
    // coffeeMachine.prepareBeverage("hot_tea");
    // coffeeMachine.prepareBeverage("black_tea");
    // coffeeMachine.refill("hot_water", 200);
    // coffeeMachine.refill("sugar_syrup",100);
    // coffeeMachine.prepareBeverage("green_tea");
    // coffeeMachine.prepareBeverage("hot_coffee");
    // cout<<"\n********************************\n";


    fin.close();
    return 0;
}