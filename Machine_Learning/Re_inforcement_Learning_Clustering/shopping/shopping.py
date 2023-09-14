import csv
import sys
from classes import evidence_class as EC

from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier

TEST_SIZE = 0.4



def main():

    # Check command-line arguments
    if len(sys.argv) != 2:
        sys.exit("Usage: python shopping.py data")

    # Load data from spreadsheet and split into train and test sets
    evidence, labels = load_data(sys.argv[1])
    X_train, X_test, y_train, y_test = train_test_split(
        evidence, labels, test_size=TEST_SIZE
    )

    # Train model and make predictions
    model = train_model(X_train, y_train)
    predictions = model.predict(X_test)
    sensitivity, specificity = evaluate(y_test, predictions)

    # Print results
    print(f"Correct: {(y_test == predictions).sum()}")
    print(f"Incorrect: {(y_test != predictions).sum()}")
    print(f"True Positive Rate: {100 * sensitivity:.2f}%")
    print(f"True Negative Rate: {100 * specificity:.2f}%")


def load_data(filename):
    """
    Load shopping data from a CSV file `filename` and convert into a list of
    evidence lists and a list of labels. Return a tuple (evidence, labels).

    evidence should be a list of lists, where each list contains the
    following values, in order:
        - Administrative, an integer
        - Administrative_Duration, a floating point number
        - Informational, an integer
        - Informational_Duration, a floating point number
        - ProductRelated, an integer
        - ProductRelated_Duration, a floating point number
        - BounceRates, a floating point number
        - ExitRates, a floating point number
        - PageValues, a floating point number
        - SpecialDay, a floating point number
        - Month, an index from 0 (January) to 11 (December)
        - OperatingSystems, an integer
        - Browser, an integer
        - Region, an integer
        - TrafficType, an integer
        - VisitorType, an integer 0 (not returning) or 1 (returning)
        - Weekend, an integer 0 (if false) or 1 (if true)

    labels should be the corresponding list of labels, where each label
    is 1 if Revenue is true, and 0 otherwise.
    """
    evidence = parse_csv(filename)
    return evidence
    raise NotImplementedError

def parse_csv(filename):
    evidence_list = []
    label_list = []

    with open(filename,'r') as file:
        csv_reader = csv.DictReader(file)
        # Informational,Informational_Duration,ProductRelated,ProductRelated_Duration,
        # BounceRates,ExitRates,PageValues,SpecialDay,Month,
        # OperatingSystems,Browser,Region,TrafficType,VisitorType,Weekend,Revenue

        # Assigning all variables by using the classes
        for row in csv_reader:
            value0 = row['Administrative']
            value1 = row['Administrative_Duration']
            value2=  row['Informational']
            value3 =row['Informational_Duration']
            value4 =row['ProductRelated']
            value5 =row['ProductRelated_Duration']
            value6 =row['BounceRates']
            value7 =row['ExitRates']
            value8 =row['PageValues']
            value9 =row['SpecialDay']
            value10 =row['Month']
            value11 =row['OperatingSystems']
            value12 =row['Browser']
            value13 =row['Region']
            value14 =row['TrafficType']
            value15 =row['VisitorType']
            value16 =row['Weekend']
            value17 =int(row['Revenue'] == 'TRUE')

            # Appending evidence and label to their respective list
            evidence_list.append(EC(value0,value1,value2,value3,value4,value5,value6,value7,value8,value9,value10,value11,value12,value13,value14,value15,value16).list_evidence())
            label_list.append(value17)

            #print("evidence_list",evidence_list,"label_list",label_list, end = 'END!!!!!!!! \n')

    return (evidence_list, label_list)

def train_model(evidence, labels):
    """
    Given a list of evidence lists and a list of labels, return a
    fitted k-nearest neighbor model (k=1) trained on the data.
    """

    # Calling k-Classifeir neighbors function
    classifier = KNeighborsClassifier(n_neighbors=1)
    # Fitting the model
    classifier.fit(evidence,labels)

    # Returning the classifier
    return classifier
    raise NotImplementedError


def evaluate(labels, predictions):
    """
    Given a list of actual labels and a list of predicted labels,
    return a tuple (sensitivity, specificity).

    Assume each label is either a 1 (positive) or 0 (negative).

    `sensitivity` should be a floating-point value from 0 to 1
    representing the "true positive rate": the proportion of
    actual positive labels that were accurately identified.

    `specificity` should be a floating-point value from 0 to 1
    representing the "true negative rate": the proportion of
    actual negative labels that were accurately identified.
    """

    # Initializing all the variables to Zero
    true_positives = 0
    true_negatives = 0
    false_positives = 0
    false_negatives = 0

    # Making a confusion matrix
    for actual_data, predicted_data in zip(labels,predictions):
        if actual_data == 1:
            if predicted_data == 1:
                true_positives += 1
            else:
                false_negatives += 1
        else:
            if predicted_data == 1:
                false_positives += 1
            else:
                true_negatives += 1

        


    # Calculating sensitivity and specificity
    sensitivity = float(true_positives/ (true_positives+false_negatives))
    specificity = float(true_negatives/ (true_negatives+false_positives))

    # returning two things at the same time
    return sensitivity,specificity
    raise NotImplementedError


if __name__ == "__main__":
    main()
