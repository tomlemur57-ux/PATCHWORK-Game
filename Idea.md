# PATCHWORK

*A puzzle game where you move the rooms(the Windwos windows), not just the robot.*


## The game

A little maintenance robot is stranded in a broken machine. Its rooms have come apart into separate desktop windows. Drag those windows together, line up their doors, and help the robot carry a battery to the exit.

The important moment: the robot walks out of one **actual window** and into another. Later, a room with sideways gravity lets you send a battery through a doorway that you could not reach before.(The game only works on Windows where you move your Windows windows)

The desktop is the space between the rooms. The game controls only its own windows. 


## What inspired it

[Windowkill][1] makes moving, shrinking windows part of a shooter. [Windowframe][2] makes window boundaries part of its platforming rules. Borrow the idea that the application window can be a game object, not their levels, art, combat, or code.

The Game name is ai generated tho, not the game(ai assisted mostly)

## Rules worth keeping consistent

- **Rooms have fixed dimensions initially.** Rearranging them changes their connections, not their internal geometry. Resizing is not a first-release mechanic.(but i will somehow add resizing of windows later with some mechanic)
- **Only aligned, opposing doorways connect.** Overlapping windows do not become portals. An unconnected doorway stays shut; snapping should not require pixel-perfect dragging.
- **An object belongs to one room at a time.** Crossing preserves its motion in screen coordinates; the destination room's gravity then takes over. Moving a window does not automatically fling its contents.
- **Arrange mode pauses the whole simulation.** Reposition rooms, then explicitly return to Run. Live dragging and catching falling objects remain experiments until the prototype proves they work reliably. (this is because i cant get it to work when actively moving windows and the character, but i will probably fix it)
- **A blocked destination stays blocked.** Do not transfer the robot inside a wall or duplicate a battery. Disconnecting a doorway during a crossing needs a defined safe result, not a lost object.

Suggested initial controls: A/D or arrow keys to move, Space to jump, E to pick up or put down a battery, Tab to switch Arrange/Run, R to restart, Esc for the menu. Show the relevant control when it becomes useful, not a full manual at startup.

Provide a click-to-select/click-to-place alternative to dragging, plus keyboard room movement in Arrange mode. These are useful accessibility goals; W3C's dragging guidance explains why dragging alone can exclude people.[8]

## A small game, then more

| Stage | What the player gets | What must work before expanding |
|---|---|---|
| Two-room prototype | Connect doors and walk across. | Native windows, focus, snapping, transfer and restart. |
| Three-puzzle demo | Carry a battery, reuse a room as a bridge, then try one gravity room. | Each puzzle teaches or recombines an understandable rule. |
| First release | Aim for six to eight good puzzles, local progress, settings and credits. | Playtests, a Windows export and recovery from awkward window positions. |
| Later experiments | Live-motion catching, a small room editor and shareable puzzle files. | Evidence that the extra complexity makes the game better. |

Possible puzzle names: **Wrong address**, **Extension lead**, **Sideways delivery**. Names can come after the puzzles work.

Do not add accounts, multiplayer, procedural campaigns, achievements, a marketplace, or an online service to the first release. A short game with several good discoveries is enough.

## How it should look

### A repair-bench toy

The rooms should resemble small, repaired pieces of machinery: enamel panels, visible door hinges, chunky sockets and a few screwed-on labels. Not a futuristic dashboard and not a fake operating system.

The robot has a squat body, two wheels and a rectangular battery slot. Keep its silhouette readable before adding a face. A bent antenna or one replacement wheel can give it personality without requiring dozens of animations.

Use a small, shared set of shapes and textures. A pump room and a charging room can have different machinery, but their doors and controls must speak the same visual language.

**Starting palette — a proposal to test, not a finished accessibility specification:**

| Role | Colour |
|---|---|
| Panel enamel | `#DDE3D9` |
| Outlines and main text | `#20252A` |
| Machinery | `#3559B8` |
| Robot body | `#D84F42` |
| Batteries and powered parts | `#E8BB39` |
| Light labels | `#F8F6ED` |

Use dark text on light panels. Do not assume every combination is readable. Hazard markings need a shape or hatch pattern; powered sockets need a visible state change, not only a new colour. Pair important sounds with visible feedback.[7]

Start the art on a 16-pixel tile grid, with a roughly 16-by-24-pixel robot. Keep sprite scaling consistent and test the result at actual window sizes. Render interface text separately so it can stay legible at larger settings.

Use one readable UI typeface, regular and bold. The prototype can use Godot's default font. Draw a simple PATCHWORK wordmark later instead of building a custom font system. Distinctive typography does not mean difficult-to-read instructions.

Give the game a few specific reactions: a connector closes with a short clack; a loose battery rattles on landing; the robot braces before a jump. These are targets for animation and audio, not assets that already exist.

**Avoid:** unrelated icon packs, mismatched pixel sizes, decorative code rain, glass panels, continuous glow, gratuitous particles, and an elaborate landing page before there is a game. A limited palette is useful only when the shapes, controls and animation also agree.

## How it should sound in writing

Write as though you are showing somebody the game at your desk. Describe what they can do. Do not announce how immersive, seamless, innovative or revolutionary it is.

| Instead of | Write |
|---|---|
| “Experience a revolutionary multi-window adventure.” | “Move the windows. Make a path for the robot.” |
| “Dynamic spatial interconnectivity.” | “Line up the doors.” |
| “Invalid portal configuration.” | “The other doorway is blocked.” |
| “Begin your journey.” | “Play.” |
| “Optimize your workspace arrangement.” | “Bring the rooms back on screen.” |

Keep terminology stable: rooms, doors, batteries, Arrange and Run. A door should not suddenly become a portal, connector or traversal node in player-facing text.

A little dry humour belongs on a room label or in the robot's reaction. Instructions and errors should remain useful. Keep jokes out of essential instructions and error messages.

Devlogs should report actual observations: what went wrong, what changed, and what still needs work. A screenshot of a real bug and its fix is more useful than a paragraph about “significant progress.” Do not use example copy as evidence that something happened.

**Possible project-page copy, once the described build exists:**

> The exit is in the wrong window.
>
> PATCHWORK is a small puzzle game about a robot, a battery, and rooms that won't stay put. Drag the windows together, line up their doors, and find a way through.

Lead the page with a short recording of real window-crossing gameplay. Follow it with the download, controls, supported systems, credits and known problems. No invented testimonials or renders presented as gameplay.

## Technical notes for the prototype

Use Godot 4.x and GDScript; record the exact version used. Godot's `Window` supports native and embedded windows.[6] That confirms the building block, not that PATCHWORK's interactions already work.

Keep one game process with a central room manager. Start with two native room windows and simple geometry. Use a consistent coordinate conversion between screen-space door positions and each room's local simulation. Commit transfers once, at a physics boundary, after checking the destination.

**Test the risky part first:** dragging, window decorations, focus changes, key releases, snapping and object transfer on the actual Windows machine. Try display scaling at 100%, 125%, 150% and 200%. Check small screens before designing four-room puzzles. Mixed-DPI multi-monitor play is a later compatibility target, not an initial promise.

Use the operating system's normal frame first. Custom frames come only after movement and focus are dependable. If native dragging interrupts simulation, keep Arrange/Run intentional rather than promising real-time physics it cannot deliver. An embedded-window practice mode can be a fallback, but should not be advertised as native desktop gameplay.

Esc should always lead to a clear way to quit. Handle window-close requests deliberately; restore displaced rooms; pause when the application genuinely loses focus. Do not steal focus from other applications or require administrator access. Save only the game's own settings and progress.

A useful test set includes crossing in both directions, holding movement across a transfer, carrying a battery, restarting while it is airborne, a blocked destination, rapidly reconnecting rooms, and losing focus during movement. Test gravity changes only after ordinary crossing is reliable.

## Next task

Make two plain windows, connect their doors, and move one square between them. Everything else can wait until that works and feels interesting.

Keep the source art, meaningful commits, real test results and short session notes. Credit any reused code, assets and project inspirations accurately.

## Sources

Reference material from the original project draft. Game mechanics, palette and scope above are proposed design decisions, not official Hack Club requirements.

[1]: https://torcado.itch.io/windowkill "Windowkill — creator's project page"
[2]: https://managore.itch.io/windowframe "Windowframe — creator's project page"
[3]: https://learn.microsoft.com/en-us/style-guide/brand-voice-above-all-simple-human "Microsoft Writing Style Guide"
[4]: https://dukope.itch.io/mars-after-midnight/devlog/285964/working-in-one-bit "Lucas Pope — Working in One Bit"
[5]: https://dukope.itch.io/mars-after-midnight/devlog/440614/typography "Lucas Pope — Typography"
[6]: https://docs.godotengine.org/en/stable/classes/class_window.html "Godot — Window documentation"
[7]: https://learn.microsoft.com/en-us/xbox/accessibility/xbox-accessibility-guidelines/103 "Xbox Accessibility Guideline 103"
[8]: https://www.w3.org/WAI/WCAG22/Understanding/dragging-movements.html "W3C — Dragging Movements"

1. [Windowkill][1] — window-based gameplay reference.
2. [Windowframe][2] — window-boundary platforming reference.
3. [Microsoft writing guidance][3] — plain language and clear next steps.
4. [Working in One Bit][4] — small-screen art, legibility and iteration.
5. [Typography][5] — a game developer's type-system decisions.
6. [Godot Window][6] — native/embedded windows and close handling.
7. [Xbox accessibility guidance][7] — communicating important cues in more than one way.
8. [W3C dragging guidance][8] — alternatives to drag-only interaction.
