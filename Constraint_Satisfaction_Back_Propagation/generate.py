import sys

import warnings

from crossword import *
from collections import deque



warnings.filterwarnings("ignore", category=DeprecationWarning, message=".*textsize is deprecated.*")

class CrosswordCreator():

    def __init__(self, crossword):
        """
        Create new CSP crossword generate.
        """
        self.crossword = crossword
        self.domains = {
            var: self.crossword.words.copy()
            for var in self.crossword.variables
        }

    def letter_grid(self, assignment):
        """
        Return 2D array representing a given assignment.
        """
        letters = [
            [None for _ in range(self.crossword.width)]
            for _ in range(self.crossword.height)
        ]
        for variable, word in assignment.items():
            direction = variable.direction
            for k in range(len(word)):
                i = variable.i + (k if direction == Variable.DOWN else 0)
                j = variable.j + (k if direction == Variable.ACROSS else 0)
                letters[i][j] = word[k]
        return letters

    def print(self, assignment):
        """
        Print crossword assignment to the terminal.
        """
        letters = self.letter_grid(assignment)
        for i in range(self.crossword.height):
            for j in range(self.crossword.width):
                if self.crossword.structure[i][j]:
                    print(letters[i][j] or " ", end="")
                else:
                    print("█", end="")
            print()

    def save(self, assignment, filename):
        """
        Save crossword assignment to an image file.
        """
        from PIL import Image, ImageDraw, ImageFont
        cell_size = 100
        cell_border = 2
        interior_size = cell_size - 2 * cell_border
        letters = self.letter_grid(assignment)

        # Create a blank canvas
        img = Image.new(
            "RGBA",
            (self.crossword.width * cell_size,
             self.crossword.height * cell_size),
            "black"
        )
        font = ImageFont.truetype("assets/fonts/OpenSans-Regular.ttf", 80)
        draw = ImageDraw.Draw(img)

        for i in range(self.crossword.height):
            for j in range(self.crossword.width):

                rect = [
                    (j * cell_size + cell_border,
                     i * cell_size + cell_border),
                    ((j + 1) * cell_size - cell_border,
                     (i + 1) * cell_size - cell_border)
                ]
                if self.crossword.structure[i][j]:
                    draw.rectangle(rect, fill="white")
                    if letters[i][j]:
                        w, h = draw.textsize(letters[i][j], font=font)
                        draw.text(
                            (rect[0][0] + ((interior_size - w) / 2),
                             rect[0][1] + ((interior_size - h) / 2) - 10),
                            letters[i][j], fill="black", font=font
                        )

        img.save(filename)

    def solve(self):
        """
        Enforce node and arc consistency, and then solve the CSP.
        """
        print('Solving the crossword!!!!!!!!!!!??????????????/')
        self.enforce_node_consistency()
        self.ac3()
        return self.backtrack(dict())

    def enforce_node_consistency(self):
        """
        Update `self.domains` such that each variable is node-consistent.
        (Remove any values that are inconsistent with a variable's unary
         constraints; in this case, the length of the word.)
        """

        # Need to know all the lengths/unary constraints of all the variables to filter out the domains
        # For Testing:

        for variables, domain_set in self.domains.items():
            length_filter = variables.length
            domain_set_copy = domain_set.copy()
            for value in domain_set_copy:
                if len(value) != length_filter:
                    domain_set.remove(value)
        
        # raise NotImplementedError

    def constraint_checker(x,y,overlap_information):

        i,j = overlap_information

        if x[i] != y[j]:
            #print("False")
            return False
        else:
            #print("True")
            return True
        # raise NotImplementedError

    def revise(self, x, y):
        """
        Make variable `x` arc consistent with variable `y`.
        To do so, remove values from `self.domains[x]` for which there is no
        possible corresponding value for `y` in `self.domains[y]`.

        Return True if a revision was made to the domain of `x`; return
        False if no revision was made.
        """
        revised = False
        a,b = self.crossword.overlaps[x,y]
        # print("a: ",a,", b: ",b)
        overlap_information = (a,b)
        for little_x in self.domains[x]: 
            if not any(self.constraint_checker(little_x,little_y,overlap_information) for little_y in self.domains[y]):
                self.domains[x].remove(little_x)
                revised = True
        return revised 
        # raise NotImplementedError

    def generate_arcs(self):
        list_of_arcs = deque()

        for x in self.crossword.variables:
            # print("x",x)
            for y in self.crossword.neighbors(x):
                # print('y: ', y)
                list_of_arcs.append((x,y))
            
        # print('list: ',list_of_arcs)
        return list_of_arcs
                   


    def ac3(self, arcs=None):
        """
        Update `self.domains` such that each variable is arc consistent.
        If `arcs` is None, begin with initial list of all arcs in the problem.
        Otherwise, use `arcs` as the initial list of arcs to make consistent.

        Return True if arc consistency is enforced and no domains are empty;
        return False if one or more domains end up empty.
        """
        # !!!!!!!!!!! PROBLEMMMMM !!!!!!!
        # print('arcs: ',arcs)
        arcs_queue = []
        if arcs is None:
            # Make a generate arcs function
            arcs_list =  self.generate_arcs()
            arcs_queue = arcs_list
            # print(" FALSE IN AC3 ")
            # print(arcs_queue)
        else:
            
            arcs_queue = deque(arcs)
        
        while not arcs_queue:
            # print("inside_arcs_queue loop")
            (X,Y) = arcs_queue.popleft() 
            if self.revise(arcs,X,Y):
                # print("Hello, World")
                if len(self.domains[X]) == 0:
                    return False
                for Z in self.crossword.neighbors(X)-{Y}:
                    arcs_queue.append((Z,X))
        return True
        # raise NotImplementedError

    def assignment_complete(self, assignment):
        """
        Return True if `assignment` is complete (i.e., assigns a value to each
        crossword variable); return False otherwise.
        """
        # print('inside assign_complete function')
        print('inside assign_complete function, assignment: ',assignment)

        if not assignment:
            return False
        

        if len(self.crossword.variables) != len(assignment.items()):
            return False
    

        for variable, assigned_value in zip(self.crossword.variables,assignment.items()):
            variable_assigned, values = assigned_value
            print(variable,': ',values)
            if values is None:
                # print('Cross word is incomplete')
                return False
            
            if variable not in assignment.keys():
                # print("variable that sohuld be in assignment isn't")
                return False

            
        # print('Crossword is complete')
        return True

        # Will be Removed for Testing ONLY! Be Careful
        # raise NotImplementedError

    def consistent(self, assignment):
        """
        Return True if `assignment` is consistent (i.e., words fit in crossword
        puzzle without conflicting characters); return False otherwise.
        """
        print('Inside consistent function')
        # Objectives: 
        # All values must be distinct : Make a set and if the length of the set is different than the length of the list of values in the assignment dict then one or more values is not distinct and return false
        # Every value should be the correct length : Not sure exactly where We will get the correct length from, but comparing lengths shouldn't be too bad
        # There should not be any conflicts between neighboring variables : Thinking....

        # Solution:

        
        distinct_set = set((assignment.values()))
        if len(distinct_set) != len(assignment.values()):
            return False

        for variable, value in assignment.items():
            print('variable: ', variable, ' value: ', value)
            print('length of variable: ', variable.length, 'length of value: ', len(value))
            if variable.length != len(value):
                # print(' The length is not same')
                return False
            neigbor_words_set = self.crossword.neighbors(variable)
            
            print(' all neighbors: ', neigbor_words_set)
            print('assignment: ', assignment)
            # conflicts = any(self.overlap_DoWe(variable,neighbor,value,assignment.get(neighbor))for neighbor in neigbor_words_set)
            for neighbor in neigbor_words_set:
               conflict = self.overlap_DoWe(variable,neighbor,value,assignment.get(neighbor))
               print('neighbor checking for: ', neighbor)
               if conflict == False:
                   return False
            
            # if not any(self.overlap_DoWe(variable,neighbor,value,assignment.get(neighbor)) for neighbor in neigbor_words_set):
            #     return False
        return True
        # raise NotImplementedError

    def overlap_DoWe(self,variable, neighbor,value1,value2):
        print('Inside of checking for overlap function')
        cell = self.crossword.overlaps[variable,neighbor]
        i,j = cell
        print(value1,': ',i,' v. ', value2,': ',j)
        if value2 is None:
            return True
        elif value1[i] != value2[j]:
            print('False, not consistent')
            return False
        else:
            return True

    
    def count_word_out(self,overlap_information, assignment,value, variable):
        i,j = overlap_information
        count = 0
        for value_list in self.domains[variable]:
            if value[i] != value_list[j]:
                count += 1
            # print("Hello, World!")
        
        return count

    def order_domain_values(self, var, assignment):
        """
        Return a list of values in the domain of `var`, in order by
        the number of values they rule out for neighboring variables.
        The first value in the list, for example, should be the one
        that rules out the fewest values among the neighbors of `var`.
        """
        #print('inside of order_domain_values function, value of var: ', var)
        #print(' Domain of var: ', self.domains[var])
        # previous loop
        # return self.domains[var]
        domain_list = dict()
        lcv_heuristic = 0
        for value_1 in self.domains[var]:
            lcv_heuristic = 0
            for neighbor in self.crossword.neighbors(var):
                if neighbor in assignment:
                    continue
                for value_2 in self.domains[neighbor]:
                    conflict = self.overlap_DoWe(var, neighbor, value_1, value_2)
                    
                    if conflict:
                        lcv_heuristic += 1
        
            domain_list[value_1] = lcv_heuristic
        
        # Sorting the dictionary based on the valeus in ascending order
        sorted_dict = dict(sorted(domain_list.items(), key=lambda item: item[1]))

        # Getting all the keys from the sorted dictionary
        keys = sorted_dict.keys()

        # Making all the values in a list
        domains_var = list(keys)

        # returning it in the correct order
        return domains_var


    def select_unassigned_variable(self, assignment):
        """
        Return an unassigned variable not already part of `assignment`.
        Choose the variable with the minimum number of remaining values
        in its domain. If there is a tie, choose the variable with the highest
        degree. If there is a tie, any of the tied variables are acceptable
        return values.
        """

        # print('inside of select unassigned_variable function')
        # print('dictionary: ', self.crossword.variables)
        list_variables = []
        for variable in self.crossword.variables:
            # Make a dictionary if variable is not in assignmnet yet, means that the variable is not assigned yet. 
            if variable not in assignment:
                # Make a dictionary:
                dictionary_variable = dict()
                dictionary_variable["variable"] = variable
                dictionary_variable["domain"] = len(self.domains[variable])
                dictionary_variable["neighbors"] = len(self.crossword.neighbors(variable))
                # Adding the new dictionary to the list
                list_variables.append(dictionary_variable)
            
        # Sorting the list based on the domain in the ascending order and in descending order based on the number of neighbors
        if list_variables is not None:
            sorted_data = sorted(list_variables, key=lambda x: (x["domain"], -x["neighbors"]))
            unassigned_variable = sorted_data[0]["variable"]
            return unassigned_variable

        # Else return None as there are no choices left
        return None
        # raise NotImplementedError

    def backtrack(self, assignment):
        """
        Using Backtracking Search, take as input a partial assignment for the
        crossword and return a complete assignment if possible to do so.

        `assignment` is a mapping from variables (keys) to words (values).

        If no assignment is possible, return None.
        """
        # print('Inside of Backtrack()')
        # Check if assignment is complete:
        if self.assignment_complete(assignment):
            # print('Complete Assignment: ', assignment)
            return assignment
        # Try a new variable:
        var = self.select_unassigned_variable(assignment)
        # print('Unassigned Variable: ', var)
        # For loop to go voer all values in DOMAIN-VALUES
        # print(' Domain of values fo rthat varibale: ',self.order_domain_values(var,assignment))
        for value in self.order_domain_values(var,assignment):
            #new_assignment = assignment.copy()
            #new_assignment[var] = value
            assignment[var] = value
            # print('Updated Assignment: ', assignment)
            # Checking for Consistency
            if self.consistent(assignment):
                # print('It is consistent!!!!!!!!!!!!!!!!!!!!!!!!!')
                result = self.backtrack(assignment)
                if result:
                    return result
            assignment.pop(var, None)
        return None
        # raise NotImplementedError


def main():


    # Check usage
    if len(sys.argv) not in [3, 4]:
        sys.exit("Usage: python generate.py structure words [output]")

    # Parse command-line arguments
    structure = sys.argv[1]
    words = sys.argv[2]
    output = sys.argv[3] if len(sys.argv) == 4 else None

    # Generate crossword
    crossword = Crossword(structure, words)
    creator = CrosswordCreator(crossword)
    assignment = creator.solve()

    print('assignment: ',assignment)

    

    # Print result
    if assignment is None:
        print("No solution.")
    else:
        creator.print(assignment)
        if output:
            creator.save(assignment, output)



if __name__ == "__main__":
    main()
