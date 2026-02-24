# Cat's Crossing

The whole point of this game is to move the cat(you) to the other side and to avoid the water and cars. There are logs you can jump on. 

---

## Controls

| Button | Action |
|--------|--------|
| **D-Pad Up** | Move up one lane |
| **D-Pad Down** | Move down one lane |
| **D-Pad Left** | Move left |
| **D-Pad Right** | Move right |
| **Start** | Start game (from title) / Pause (during game) / Return to title (from Game Over or You Win) |

Movement is one step at a time (grid-based). The cat does not use A, B, or shoulder buttons.

---

## How to Play

- **Goal:** Get the cat from the bottom (starting area) to the green finish zone at the top.
- **Roads (gray/dark gray):** There are moving cars that come in from the side. Make sure to not get hit by them. 
- **Water (blue):** There are rivers with logs on them, make sure to jump on the logs and avoid the water.
- **Lives:** You start with 3 lives which are the yellow squares. Losing a life sending you back to the beginnning of the same level, the score stays the same.
- **Scoring:** Reaching the finish gives you 100 points and advances you to the next level. Cars and logs are different on the levels and it keeps going until you die. 

---

## State Machine (Screens & Flow)

1. **START (Title screen)**  
   - Shows “CAT'S CROSSING” and “PRESS START.”  
   - **Start** → start a new game.

2. **GAME (Playing)**  
   - You move the cat and try to reach the goal.  
   - **Start** → **PAUSE**.  
   - If you lose all lives then its (Game Over).

3. **PAUSE**  
   - Shows “PAUSED.”  
   - **Start** → resume **GAME**.

4. **WIN**  
   - Shows “YOU WIN” and “PRESS START.”  
   - **Start** → back to **START** (title).

5. **LOSE (Game Over)**  
   - Red screen, “GAME OVER,” levels cleared, and final score.  
   - **Start** → back to **START** (title).

So: **START → (Start) → GAME ⇄ (Start) PAUSE**, and from **GAME** you can go to **LOSE** (0 lives) or, if implemented, **WIN**. From **WIN** or **LOSE**, **Start** returns you to **START**.

---

## How You Win / How You Lose

- **Winning (per level):** To win you reach the green zone and advance to the next level. It keeps going until you die. Try to increase your score. 
- **Losing a life:**  
  - Touch a car on the road.  
  - Be in the water and not on a logg.  
  - Be on a log that moves off the left or right side of the screen so the cat goes off-screen.


---

## Bugs or Known Issues. 

- Level layout is randomized when you clear a level; cars and logs can be close together, so some runs may feel harder than others.
- Sometimes logs can be going in the same direction at the same velocity which means you can't progress
- Sometimes when you step on a log and step in the water, you don't die. 

---

## Building & Running

Build and Run using the same method as previous homeworks. 
