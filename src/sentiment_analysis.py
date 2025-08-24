"""
@file       sentiment_analysis.py
@author     Simon Håkansson
@date       2025-08-24
@brief      Standalone Python script for sentiment analysis using VADER.

@details    This script provides a function to analyze the sentiment of a given text
            and returns a compound score. It can be run independently for testing.

@copyright  Copyright (c) 2025 Simon Håkansson

This software is released under the MIT License.
See the LICENSE file in the project root for the full license text.
"""
#TODO:  VADER is a good baseline but often returns a neutral score.
#       Post-sprint, investigate a more nuanced, transformer-based models
#       to better capture the emotional range of user input.
#       This will also make the sentiment analysis work on other languages.



# -------------[ LIBRARIES ]-------------
from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer
import sys

# -------------[ FUNCTIONS ]-------------
def analyze_sentiment(text: str) -> float:
    """
    @brief  Analyzes the sentiment of a given text and returns a composite score.

    @details This function uses the VADER (Valence Aware Dictionary and sEntiment
             Reasoner) model to perform sentiment analysis on the input text.
             The score is a floating-point number between -1 (most negative) and
             +1 (most positive).

    @param text The string of text to analyze.
    @return The composite sentiment score as a float. (-1 for most negative, 0 for neutral, and 1 for most positive)
    """
    analyzer = SentimentIntensityAnalyzer()
    sentiment_scores = analyzer.polarity_scores(text)
    return sentiment_scores['compound']


# -------------[ MAIN EXECUTION FOR TESTING ]-------------
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python sentiment_analyzer.py \"Your sentence here\"")
        sys.exit(1)
    
    text_to_analyze = sys.argv[1]
    sentiment_score = analyze_sentiment(text_to_analyze)
    print(f"Sentiment Score: {sentiment_score}")