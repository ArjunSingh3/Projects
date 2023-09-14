from collections import OrderedDict
import copy
import os
import random
import re
import sys

DAMPING = 0.85
SAMPLES = 10000


def main():
    if len(sys.argv) != 2:
        sys.exit("Usage: python pagerank.py corpus")
    corpus = crawl(sys.argv[1])
    print(corpus)
    # Print Block For Reference
    #______________________________________________________________
    #"""
    sum = 0.0375+ 0.85*((0.14375/1)+((1/4)/2)+((1/4)/1))
    print("sum:",sum)
    for sub in corpus:
        print(sub)
        print("⎇  ", end="")
        iterator = len(corpus[sub])
        for sub_nest in corpus[sub]:
            iterator -= 1
            print(sub_nest)
            if iterator !=  0:
                print("⎇  ", end="")
        print()
    #"""
    #______________________________________________________________

    ranks = sample_pagerank(corpus, DAMPING, SAMPLES)
    print(f"PageRank Results from Sampling (n = {SAMPLES})")
    for page in sorted(ranks):
        print(f"  {page}: {ranks[page]:.4f}")
    ranks = iterate_pagerank(corpus, DAMPING)
    print(f"PageRank Results from Iteration")
    for page in sorted(ranks):
        print(f"  {page}: {ranks[page]:.4f}")

def crawl(directory):
    """
    Parse a directory of HTML pages and check for links to other pages.
    Return a dictionary where each key is a page, and values are
    a list of all other pages in the corpus that are linked to by the page.
    """
    pages = dict()

    # Extract all links from HTML files
    for filename in os.listdir(directory):
        if not filename.endswith(".html"):
            continue
        with open(os.path.join(directory, filename)) as f:
            contents = f.read()
            links = re.findall(r"<a\s+(?:[^>]*?)href=\"([^\"]*)\"", contents)
            pages[filename] = set(links) - {filename}

    # Only include links to other pages in the corpus
    for filename in pages:
        pages[filename] = set(
            link for link in pages[filename]
            if link in pages
        )

    return pages


def transition_model(corpus, page, damping_factor):
    """
    Return a probability distribution over which page to visit next,
    given a current page.

    With probability `damping_factor`, choose a link at random
    linked to by `page`. With probability `1 - damping_factor`, choose
    a link at random chosen from all pages in the corpus.
    """
    """
    Returns a dictionay:
    key: Page Name
    Value: Probability of each page being visited next
    """
    value = dict()
    list_of_direct_links = []
    if len(corpus[page]) != 0:
        for sub_nest in corpus[page]:
            list_of_direct_links.append(sub_nest)

        for sub in corpus:
            base_probability = (1-damping_factor)/len(corpus)
            #print(sub,":",base_probability)
            #print("⎇  ", end="")
            
            
            if sub in list_of_direct_links:
                #print(sub,"Hi :",base_probability)
                base_probability += (damping_factor/len(corpus[page]))
                #print("Damping factor: ", (damping_factor/len(corpus[page])))
                #print(sub,"Hi :",base_probability)
                
            #        #_______________________________________
            #        iterator -= 1
            #        #print(sub_nest,":",base_probability)
            #        #_______________________________________
            #        if iterator !=  0:
            #            #print("⎇  ", end="")
            #print()
            value[sub] = base_probability
        #print(value)
    else:
       # print("hello World!")
        for link in corpus:
            value[link] = 1/len(corpus)
    #print(value)
    return value
    # raise NotImplementedError


def sample_pagerank(corpus, damping_factor, n):
    """
    Return PageRank values for each page by sampling `n` pages
    according to transition model, starting with a page at random.

    Return a dictionary where keys are page names, and values are
    their estimated PageRank value (a value between 0 and 1). All
    PageRank values should sum to 1.
    """
    """
    Returns a dictionay:
    key: Page Name
    Value: Estimated Page Rank (A number between 0 and 1(Probability))
    """
    pagerank = dict()

    # Initializing all page ranks to 0 to detect errors straight forward if any
    for page in corpus:
        pagerank[page] = 0

    random_page = random.choice(list(corpus.keys()))
    pagerank[random_page] += 1
    print(random_page)
    probability_distribution = transition_model(corpus,random_page,damping_factor)
    pages_list = list(probability_distribution.keys())
    probability_list = [probability_distribution[key] for key in pages_list]
    random_page = random.choices(pages_list,probability_list)[0]
    print(random_page)
    for i in range(n):
        probability_distribution = transition_model(corpus,random_page,damping_factor)

        pages_list = list(probability_distribution.keys())
        probability_list = [probability_distribution[key] for key in pages_list]

        random_page = random.choices(pages_list,probability_list)[0]
        pagerank[random_page] += 1

    total = 0
    for key,value in pagerank.items():
        #print(key)
        #print(value)
        total += value/n
        pagerank[key] = value/n
    #print(total)
    return pagerank
    #raise NotImplementedError


def iterate_pagerank(corpus, damping_factor):
    """
    Return PageRank values for each page by iteratively updating
    PageRank values until convergence.

    Return a dictionary where keys are page names, and values are
    their estimated PageRank value (a value between 0 and 1). All
    PageRank values should sum to 1.
    """
    """
    Returns a dictionay:
    key: Page Name
    Value: Estimated Page Rank (A number between 0 and 1(Probability))
    """
    pagerank = dict()
    corpus_ordered = OrderedDict(reversed(list(corpus.items())))
    # Initialize all pages:
    for page in corpus_ordered:
        pagerank[page] = 1/len(corpus)
    '''
    check_again=False
    print('ordered corpus: ', corpus_ordered)
    print('initial pagerank: ', pagerank)
    while True:
        # break
        old_dict = dict()
    
        old_dict = copy.deepcopy(pagerank)

        for page in corpus:
            print('page first loop: ', page)
            sum = 0
        
            for other_page in corpus:
                print('pages in second loop: ', other_page)
                if page in corpus[other_page]:
                    sum += pagerank[other_page]/len(corpus[other_page])
                    
                elif not corpus[other_page]:
                    sum += pagerank[other_page]/ len(corpus)
                print('sum: ',sum)
            
            pagerank[page] = (((1-damping_factor)/len(corpus))+(damping_factor*sum))
            print('pagerank updated: page: ',page,': ',pagerank)

        for page in corpus:
            check = abs(old_dict[page]-pagerank[page])
            print('check_sum value for iterative: ', check)
            if check >= 0.001:
                check_again = True  

        if check_again:
            break
    '''
    '''Recursive Part, not needed Temporarily: '''
    #pagerank_ordered = OrderedDict(reversed(list(pagerank.items())))
    print()
    print('ordered corpus: ', corpus_ordered)
    print('initial pagerank: ', pagerank)
    pagerank= helper_iterate_pagerank(corpus_ordered,pagerank,damping_factor)
    
    
    return pagerank
    
   # print("Hello World!")
    #raise NotImplementedError

def helper_iterate_pagerank(corpus,pagerank_dict,damping_factor):
    old_dict = dict()
    
    old_dict = copy.deepcopy(pagerank_dict)
   
    for page in corpus:
        # print('page first loop: ', page)
        sum = 0
        
        for other_page in corpus:
            # print('pages in second loop: ', other_page)
        
            if page in corpus[other_page]:
                # print('NumLinks(i) ',other_page,' : ',len(corpus[other_page]))
                sum += old_dict[other_page]/len(corpus[other_page])
                
            elif not corpus[other_page]:
                sum += old_dict[other_page]/ len(corpus)
            # print('sum: ',sum)
            
        pagerank_dict[page] = (((1-damping_factor)/len(corpus))+(damping_factor*sum))
        
    call_again = False
    # print('current_dict: ', old_dict)
    # print('current_dict: ', pagerank_dict)
    # BASE CASE:
    for page in corpus:
        check = abs(old_dict[page]-pagerank_dict[page])
        # print('check_sum value for iterative: ', check)
        if check >= 0.001:
            call_again = True

    print('call again? ', call_again)

    if call_again:
        #print("iterate again")
        pagerank_dict = helper_iterate_pagerank(corpus,pagerank_dict,damping_factor)
    

    return pagerank_dict
            

    



if __name__ == "__main__":
    main()
