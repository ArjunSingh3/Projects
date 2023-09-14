import nltk
import sys
import os
import string
import numpy as np
import math
from collections import OrderedDict

from nltk.corpus import stopwords

FILE_MATCHES = 5
SENTENCE_MATCHES = 10

corpus_directory = os.path.join(os.getcwd(),'corpus')

def main():

    # Check command-line arguments
    if len(sys.argv) != 2:
        sys.exit("Usage: python questions.py corpus")

    # Calculate fIDF values across files
    files = load_files(sys.argv[1])
    file_words = {
        filename: tokenize(files[filename])
        for filename in files
    }
    file_idfs = compute_idfs(file_words)

    # Prompt user for query
    query = set(tokenize(input("Query: ")))

    # Determine top file matches according to TF-IDF
    filenames = top_files(query, file_words, file_idfs, n=FILE_MATCHES)

    # Extract sentences from top files
    sentences = dict()
    for filename in filenames:
        for passage in files[filename].split("\n"):
            for sentence in nltk.sent_tokenize(passage):
                tokens = tokenize(sentence)
                if tokens:
                    sentences[sentence] = tokens

    # Compute IDF values across sentences
    idfs = compute_idfs(sentences)

    # Determine top sentence matches
    matches = top_sentences(query, sentences, idfs, n=SENTENCE_MATCHES)
    for match in matches:
        print(match)


def load_files(directory):
    """
    Given a directory name, return a dictionary mapping the filename of each
    `.txt` file inside that directory to the file's contents as a string.
    """
    corpus_dictionary = {}

    for file_name in os.listdir(corpus_directory):
        file_path = os.path.join(corpus_directory,file_name)
        #print("Hello World")
        #print("file_name: ",file_name)
        with open(file_path, 'r') as file:
            content = file.read()
        
        #print("Content: ",content)
        corpus_dictionary[file_name] = content

    return corpus_dictionary
    # raise NotImplementedError


def tokenize(document):
    """
    Given a document (represented as a string), return a list of all of the
    words in that document, in order.

    Process document by coverting all words to lowercase, and removing any
    punctuation or English stopwords.
    """

    tokens = nltk.word_tokenize(document)

    stop_words = set(stopwords.words("english"))

    sentence = [word for word in tokens if not word in string.punctuation and word not in stop_words]
    sentence_lowercase = [word.lower() for word in sentence]
    #print('tokenize return list: ',sentence_lowercase)
    return sentence_lowercase
    raise NotImplementedError

    
def compute_idfs(documents):
    """
    Given a dictionary of `documents` that maps names of documents to a list
    of words, return a dictionary that maps words to their IDF values.

    Any word that appears in at least one of the documents should be in the
    resulting dictionary.
    """

    """
    {
        document1:['Hello', 'World'], 
        document2:['Hi','How' ,'are' ,'you', 'doing'], 
        document3:['Hi', 'How', 'are', 'you','doing','i','am','veronica','you're','personal','assistant']
        
    }
    """

    """
    import numpy as np

    # Assuming you have a map that maps document names to the list of words inside each document.
    # Example:
    document_map = {
        'doc1': ['word1', 'word2', 'word3'],
        'doc2': ['word2', 'word3', 'word4'],
        'doc3': ['word3', 'word5', 'word6'],
        # ... and so on
    }

    # Create a flat list of all words from all documents using a list comprehension.
    
    
    all_words = [word for words in document_map.values() for word in words]

    # Get unique words and their counts using NumPy's unique function.
    
    unique_words, word_counts = np.unique(all_words, return_counts=True)

    # Create a dictionary to store the number of documents each word appears in.
    
    word_document_count = {}

    # Use NumPy to count the number of documents each word appears in.
    
    for word in unique_words:
        word_document_count[word] = np.sum([word in words for words in document_map.values()])

    # Now you have a dictionary (word_document_count) that maps each word to the number of documents it appears in.
    # You can access the count for a specific word using word_document_count[word].

    
    """
    
    # NEsted for loop: Time Complexity: O(number of words * number of documents)
    all_words = [word for words in documents.values() for word in words]
    
    unique_words, word_count = np.unique(all_words, return_counts = True)

    word_document_count = {}

    number_of_documents = len(documents)

    for word in unique_words:
        word_document_count[word] = math.log(number_of_documents/np.sum([word in words for words in documents.values()]))

    return word_document_count

    raise NotImplementedError


def top_files(query, files, idfs, n):
    """
    Given a `query` (a set of words), `files` (a dictionary mapping names of
    files to a list of their words), and `idfs` (a dictionary mapping words
    to their IDF values), return a list of the filenames of the the `n` top
    files that match the query, ranked according to tf-idf.
    """
    tfidfs = dict()



    corpus = tf_calculator(files)


    for filename in corpus:
        tfidfs[filename] = []
        for word in corpus[filename]:
            tf = corpus[filename][word]
            if word in query:
                tfidfs[filename].append((word,tf * idfs[word]))
            else:
                continue
    
    
    top_list = top_n(tfidfs,n)
    # Print statement to check the number of files being returned:
    print('top files ',n,': ',top_list)
    return top_list

    raise NotImplementedError

'''
SENTENCE CLASSIFICATION
'''
def top_sentences(query, sentences, idfs, n):
    """
    Given a `query` (a set of words), `sentences` (a dictionary mapping
    sentences to a list of their words), and `idfs` (a dictionary mapping words
    to their IDF values), return a list of the `n` top sentences that match
    the query, ranked according to idf. If there are ties, preference should
    be given to sentences that have a higher query term density.
    """

    # print("idfs: ",idfs)
    #print("Sentneces: ",sentences)
    sentence_list = []
    # print("Query: ",query)

    for sentence_Obj in sentences.items():
        sentence, words = sentence_Obj
        idf_sum = 0
        #print("new sentence")
        for word in query:
            # print("word: ", word)
            lowercase_words = [word.lower() for word in words]
            if word in lowercase_words:
                # print(query)
                # print("comparison: ", word)
                # print(sentence)
                #print(idf_sum)
                idf_sum += idfs[word]
                #print("word: ",word, "idf_value: ", idfs[word], "sum: ", idf_sum)
            else:
                continue
        # idf_sum = sum(idfs[word] for word in query if word in words)
        # print("idf_sum: ", idf_sum)
        density = len(set(words) & set(query))/ len(sentence)

        top_sentence_obj = dict()
        top_sentence_obj['sentence_Obj'] = sentence
        top_sentence_obj['idf_sum'] = idf_sum
        top_sentence_obj['qtm_density'] = density

        sentence_list.append(top_sentence_obj)

    if sentence_list is not None:
        sentence_list.sort(key=lambda x: (x["idf_sum"], x["qtm_density"]), reverse=True)

    sentences_only = [sentence_data["sentence_Obj"] for sentence_data in sentence_list]
    # Print statement to check the number if sentences being returned
    # print('length of sentences_only list: ', len(sentences_only[:n]))
    # print('top sentences ',n,':' ,sentences_only[:n])
    return sentences_only[:n]

    '''
        idf_sum=0
        for word in words:
            if word in query:
                #print("word: ",word, " idf Value: ", idfs[word])
                idf_sum+=idfs[word]

        '''

        # print("Sum: ", idf_sum)
        #sentences = (sentence,tuple(words))
        #sentence_list[sentences] = idf_sum


    '''    
    
    sorted_dict = OrderedDict(sorted(sentence_list.items(), key=lambda x: x[1], reverse = True))
    # top_n_sentence = sentence_list.sort(key=lambda sentence_idf: sentence_idf[1], reverse=True)
   
    #print(list(sorted_dict.items())[:10])
    # print('before sorting based on sentence density: ', sorted_dict)
    top_element_value = next(iter(sorted_dict.values()))
    # print("Top element value: ", top_element_value)
    top_sentences =dict()

    for sentences, idf_value in sorted_dict.items():
        sentence, words = sentences
       
        try:
            density = len(set(words) & set(query))/ len(sentence)
            # print("Density: ", density)
        except ZeroDivisionError:
            print("Not valid")
            
        top_sentences[sentence] = density
        

    sorted_sentence = OrderedDict(sorted(top_sentences.items(), key=lambda x: x[1], reverse = True))
    print('sorted sentenceS: ',sorted_sentence)

    top_sentence = list(sorted_sentence.items())[:n]
    # Print statement to check the number if sentences being returned
    print('top sentences ',n,':' ,top_sentence)
    return top_sentence

    


    # print(top_10_elements)
    top_n_sentence = list(sorted_dict.keys())[:n]
    

    # print(top_n_sentence)
    return top_n_sentence

    raise NotImplementedError
    '''

'''
TF-CALCULATOR FUNCTION
'''
def tf_calculator(file_dictionary):
    
    files = dict()
    for filename in file_dictionary:
        frequencies = dict()
        for word in file_dictionary[filename]:
            if word not in frequencies:
                frequencies[word] =1
            else:
                frequencies[word] += 1
        files[filename] = frequencies

    return files

'''
MAXIMUM FUNCTION
'''
def top_n(tfidfs_dict,n):

    top_n_list = dict()
    for filename in tfidfs_dict:
        top_n_list[filename] = sum(item[1] for item in tfidfs_dict[filename])
    
    sorted_dict = OrderedDict(sorted(top_n_list.items(), key=lambda x: x[1], reverse = True))
    # top_n_list.sort(key=lambda tfidf: tfidf[1], reverse = True)
    # top_n_list = sorted_dict[:n]]
    top_n_elements = list(sorted_dict.keys())[:n]
    
    return top_n_elements

         

if __name__ == "__main__":
    main()