#include <iostream>
#include <string> 
#include <cstdlib>  
#include <ctime>  
#include <limits> 
#include <random>

enum Suit { HEARTS, DIAMONDS, CLUBS, SPADES, SUIT_COUNT };
enum Rank { TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE, RANK_COUNT };
static const int DECK_SIZE = SUIT_COUNT * RANK_COUNT;

class Card {
public:
    Suit suit;
    Rank rank;
    Card* next;

    public:
    // Constructor
    Card(Suit s, Rank r) : suit(s), rank(r), next(nullptr) {}

    // Accessor for the suit
    Suit getSuit() const {
        return suit;
    }

    // Accessor for the rank
    Rank getRank() const {
        return rank;
    }

    // Return the value of the card
    int getValue() const {
        if (rank == ACE) return 11;
        if (rank >= JACK) return 10;
        return static_cast<int>(rank) + 2;  // TWO returns 2, THREE returns 3, etc.
    }

    // Accessor and mutator for the next pointer
    Card* getNext() const {
        return next;
    }

    void setNext(Card* card) {
        next = card;
    } 

     void displayCard() const {
        std::cout << rankToString(rank) << " of " << suitToString(suit) << std::endl;
    }

private:
    static std::string rankToString(Rank r) {
        switch (r) {
            case TWO:   return "Two";
            case THREE: return "Three";
            case FOUR:  return "Four";
            case FIVE:  return "Five";
            case SIX:   return "Six";
            case SEVEN: return "Seven";
            case EIGHT: return "Eight";
            case NINE:  return "Nine";
            case TEN:   return "Ten";
            case JACK:  return "Jack";
            case QUEEN: return "Queen";
            case KING:  return "King";
            case ACE:   return "Ace";
            default:    return "Unknown Rank"; 
        }
    }

    static std::string suitToString(Suit s) {
        switch (s) {
            case HEARTS:   return "Hearts";
            case DIAMONDS: return "Diamonds";
            case CLUBS:    return "Clubs";
            case SPADES:   return "Spades";
            default:       return "Unknown Suit";
        }
    }
}; 

class Deck {
private:
    Card* head;

public:
    Deck() : head(nullptr) {
        reset();  // Initialize the deck with a full set of cards
    }

    ~Deck() {
        clear();  // Clear the deck upon destruction
    }

    void shuffle() {
        Card* cards[DECK_SIZE];
        Card* current = head;
        int index = 0;
        while (current) {
            cards[index++] = current;
            current = current->getNext();
        }
         
    std::random_device rd;
    std::mt19937 gen(rd()); 
    
        // Fisher-Yates Shuffle
        for (int i = DECK_SIZE - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(cards[i], cards[j]);
        }

        // Convert array back to linked list
        head = cards[0];
        current = head;
        for (int i = 1; i < DECK_SIZE; i++) {
            current->setNext(cards[i]);
            current = current->getNext();
        }
        current->setNext(nullptr); // Terminate the list
    }

    Card* deal() {
        if (!head) return nullptr;

        Card* cardToDeal = head;
        head = head->next;
        cardToDeal->next = nullptr;  // Decouple the card from the list
        return cardToDeal;
    }

    void reset() {
        clear();  // Clear the current deck first

        // Reinitialize the deck with a full set of cards
        for (int s = HEARTS; s < SUIT_COUNT; s++) {
            for (int r = TWO; r < RANK_COUNT; r++) {
                Card* newCard = new Card(static_cast<Suit>(s), static_cast<Rank>(r));
                newCard->next = head;
                head = newCard;
            }
        }
    }

private:
    void clear() {
        // Delete all cards in the deck
        while (head) {
            Card* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr; // Ensure head is reset to nullptr
    }
};

class Hand {
private:
    Card* head;

public:
    Hand() : head(nullptr) {}

    ~Hand() {
        clear(); // Clear the hand in the destructor
    }

    void addCard(Card* c) {
        c->next = head; // Add the new card at the beginning of the hand
        head = c;
    }

    int getValue() const {
        int value = 0;
        int aces = 0;
        Card* current = head;
        while (current) {
            int cardValue = current->getValue(); // Get value of the current card
            value += cardValue;
            if (cardValue == 11) aces++; // Count aces for special handling
            current = current->next;
        }

        // Handle aces (which can be 1 or 11)
        while (value > 21 && aces) {
            value -= 10; // Convert an ace from 11 to 1
            aces--;
        }

        return value;
    }

    void clear() {
        // Clear the hand by resetting the head pointer to nullptr
        head = nullptr;
    }
};


class Player {
private:
    Hand hand;

public:
    Player() {}

    // Access the player's hand
    Hand& getHand() {
        return hand;
    }

    void hit(Deck& deck) {
        Card* newCard = deck.deal();
        hand.addCard(newCard);
    }

    void stand() {
        // Usually, this will be handled in the game logic, not here. 
        // So, this might be an empty method or just a placeholder.
    }

    int getHandValue() const {
        return hand.getValue();
    }
};

class Dealer {
private:
    Hand hand;

public:
    Dealer() {}

    // Access the dealer's hand
    Hand& getHand() {
        return hand;
    }

    void playTurn(Deck& deck) {
        while (hand.getValue() < 17) {
            Card* newCard = deck.deal();
            hand.addCard(newCard);
        }
    }

    int getHandValue() const {
        return hand.getValue();
    }
};  
class GameState {
public:
    enum State { PLAYING, PLAYER_BLACKJACK, DEALER_BLACKJACK, PLAYER_BUST, DEALER_BUST, ROUND_END };

    State currentState;

    GameState() : currentState(PLAYING) {}

    State checkState(const Player& player, const Dealer& dealer) {
        if (currentState == PLAYING) {
            if (player.getHandValue() == 21) {
                currentState = PLAYER_BLACKJACK;
            } else if (dealer.getHandValue() == 21) {
                currentState = DEALER_BLACKJACK;
            } else if (player.getHandValue() > 21) {
                currentState = PLAYER_BUST;
            } else if (dealer.getHandValue() > 21) {
                currentState = DEALER_BUST;
            }
        }

        return currentState;
    }

    void announceState(const Player& player, const Dealer& dealer) const {
        switch (currentState) {
            case PLAYING:
                std::cout << "The game continues..." << std::endl;
                break;
            case PLAYER_BLACKJACK:
                std::cout << "Congratulations! You got a Blackjack!" << std::endl;
                break;
            case DEALER_BLACKJACK:
                std::cout << "Dealer got a Blackjack. Better luck next time!" << std::endl;
                break;
            case PLAYER_BUST:
                std::cout << "You busted with a hand value of " << player.getHandValue() << std::endl;
                break;
            case DEALER_BUST:
                std::cout << "Dealer busted with a hand value of " << dealer.getHandValue() << std::endl;
                break;
            case ROUND_END:
                // This is where you can check who won if no one got a Blackjack or busted
                if (player.getHandValue() > dealer.getHandValue()) {
                    std::cout << "You win with a hand value of " << player.getHandValue() << " vs dealer's " << dealer.getHandValue() << std::endl;
                } else if (player.getHandValue() < dealer.getHandValue()) {
                    std::cout << "Dealer wins with a hand value of " << dealer.getHandValue() << " vs your " << player.getHandValue() << std::endl;
                } else {
                    std::cout << "It's a tie with both having a hand value of " << player.getHandValue() << std::endl;
                }
                break;
        }
    }
}; 
class Game {
private:
    Deck deck;
    Player player;
    Dealer dealer;
    GameState gameState;

public:
    Game() : deck(), player(), dealer(), gameState() {}

    void play() {    
        gameState.currentState = GameState::PLAYING;
        player.getHand().clear();
    dealer.getHand().clear(); 
        deck.reset();
        deck.shuffle();
        // Initial deal
        player.getHand().addCard(deck.deal());
        dealer.getHand().addCard(deck.deal());
        player.getHand().addCard(deck.deal());
        dealer.getHand().addCard(deck.deal()); 

    std::cout << "Player initial hand value: " << player.getHandValue() << std::endl;
    std::cout << "Dealer initial hand value: " << dealer.getHandValue() << std::endl;


        gameState.checkState(player, dealer);
        if (gameState.currentState != GameState::PLAYING) {
            gameState.announceState(player, dealer);
            return;
        }

        // Player's turn
        while (true) {
            std::cout << "Your hand value: " << player.getHandValue() << std::endl;
            std::cout << "Do you want to (h)it or (s)tand? ";
            char choice;
            std::cin >> choice;
            if (choice == 'h') {
                player.hit(deck); 

                  std::cout << "Player hand value after hit: " << player.getHandValue() << std::endl; 

                gameState.checkState(player, dealer);
                if (gameState.currentState != GameState::PLAYING) {
                    gameState.announceState(player, dealer);
                    return;
                }
            } else if (choice == 's') {
                break;
            } else {
                std::cout << "Invalid choice. Please choose 'h' or 's'.\n";
            }
        }

        // Dealer's turn
        dealer.playTurn(deck);

        gameState.checkState(player, dealer);
        if (gameState.currentState == GameState::PLAYING) {
            gameState.currentState = GameState::ROUND_END;
        }
        gameState.announceState(player, dealer);
    }
};


int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator for shuffling

    Game blackjackGame;
    char choice;

    do {
        std::cout << "Welcome to Blackjack!" << std::endl;
        blackjackGame.play();

        std::cout << "\nWould you like to play again? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

    } while (choice == 'y' || choice == 'Y');

    std::cout << "Thanks for playing! Goodbye!" << std::endl;

    return 0;
}
