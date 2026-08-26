# Shuriken Ability Kit — Unreal Engine C++ Gameplay Coursework

A third-person Unreal Engine project implementing a **mana-gated, cooldown-based ability kit** in C++ using Unreal's Enhanced Input system — a fan-recreation of a shuriken-throwing, dash-melee-style character kit (in the spirit of League of Legends' *Talon*), built for the CMP302 module at Abertay University.

The player character has two physics-driven projectile abilities layered on top of a shared mana/cooldown economy, plus a lightweight on-screen debug UI showing ability readiness and mana in real time.

## Abilities

**Regular Shoot (Q)**
- Costs 20 mana, 3 second cooldown
- Spawns three physics-simulated shuriken projectiles in a spread pattern: one straight ahead along the player's forward vector, one rotated -30° and one rotated 220° around the player, each launched with linear velocity and independent angular spin
- After a short delay, all three shurikens automatically return to the player and are destroyed (a boomerang-style return, not a one-shot fire-and-forget)

**Ultimate**
- Costs 35 mana, 3 second cooldown
- Spawns 12 shurikens in a full 360° ring around the player using polar coordinates, each with its own spin and outward velocity
- Hides the player actor while active
- Shurikens travel outward until they reach a configurable `StandstillDistance`, at which point they stop moving but keep spinning in place, before returning to the player and despawning

**Shared systems**
- A reusable `UMyMana` **ActorComponent** (not hardcoded into the character) that tracks current/max mana, so it can be dropped onto any actor
- Passive mana regeneration (+5 every second, capped at max)
- Independent cooldown timers for each ability, decoupled from the mana cost check
- Projectiles ignore collision with the player and with each other, and destroy whatever else they overlap

## Technical highlights

- **Unreal Enhanced Input System** — Move, Look, Jump, Shoot and Ultimate are all bound through `UInputAction`/`UInputMappingContext` assets rather than legacy input bindings
- **Physics-simulated projectiles** (`AProjectile`) using `SetPhysicsLinearVelocity` / `SetPhysicsAngularVelocityInDegrees` for movement and spin, with overlap-based collision via a `UBoxComponent`
- **Component-based design**: mana is its own `UActorComponent` rather than being baked into the character class, so the system is reusable across actors
- **Timer-driven ability sequencing**: `FTimerHandle`/`GetWorldTimerManager()` used to sequence "fire → wait → auto-return → destroy" without blocking the game thread
- **Polar-coordinate spawning** for the ultimate's ring formation (`FMath::Cos`/`Sin` over evenly divided angles)
- **On-screen debug UI** built with `GEngine->AddOnScreenDebugMessage`, showing ability availability and live mana value

## Project structure

```
CMP302_Coursework-main/
├── MyProject.uproject
├── Source/MyProject/
│   ├── MyProjectCharacter.cpp/.h   # Player character: input, abilities, cooldowns, UI
│   ├── Projectile.cpp/.h           # Physics-driven shuriken projectile actor
│   ├── MyProjectGameMode.cpp/.h    # Default game mode (Third Person template)
│   ├── Public/MyMana.h             # Mana ActorComponent (reusable across actors)
│   ├── Private/MyMana.cpp
│   └── MyProject.cpp/.h            # Module entry point
├── Content/                        # Blueprints, meshes, materials, input assets
├── Config/                         # Project/engine configuration
└── Platforms/
```

## Building / running

This is an Unreal Engine C++ project (built against the Third Person template).

1. Right-click `MyProject.uproject` → **Generate Visual Studio project files** (or open directly in Unreal Editor, which will prompt to build missing modules).
2. Open the generated solution and build in Development Editor configuration, or open `MyProject.uproject` directly in the Unreal Editor and let it compile.
3. Press Play — default third-person controls apply, plus the Shoot and Ultimate actions bound via the project's Enhanced Input mapping context.

## Background

Built as coursework for CMP302 (Games Programming with Unreal) at Abertay University. The focus of the exercise was implementing a resource-gated ability system with real gameplay feel — spread-shot projectiles, a ring-formation ultimate, cooldown/mana gating, and physics-based movement — entirely in C++ on top of Unreal's Enhanced Input and physics systems, rather than relying on Blueprint scripting for the core logic.
