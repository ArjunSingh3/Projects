# Sample data structure for sentences with IDF and query term density values
sentences = [
    {"sentence": "This is sentence 1", "idf": 2.0, "query_term_density": 0.4},
    {"sentence": "Another sentence here", "idf": 1.5, "query_term_density": 0.3},
    {"sentence": "A short sentence.", "idf": 1.5, "query_term_density": 0.6},
    {"sentence": "This is a longer sentence with more words in it.", "idf": 2.0, "query_term_density": 0.2},
    {"sentence": "Sentence 5 with query terms", "idf": 2.5, "query_term_density": 0.5},
    {"sentence": "Another example sentence with query terms.", "idf": 2.5, "query_term_density": 0.4},
    {"sentence": "A sentence without query terms", "idf": 1.0, "query_term_density": 0.0},
    # Add more sentences with IDF and query term density values
]

# Sort by IDF in ascending order, and if IDF values are equal, sort by query term density
sentences.sort(key=lambda x: (x["idf"], x["query_term_density"]), reverse=True)

# Print the sorted sentences
print(sentences[0])
for sentence_data in sentences:
    print(f"Sentence: {sentence_data['sentence']}, IDF: {sentence_data['idf']}, Query Term Density: {sentence_data['query_term_density']}")
