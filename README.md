# RGL

This is my implementation of the popular Tom Looman course for C++ Mastery in Unreal Engine 5.
I will split the README in a few sections which I will be outlining what I learned through each
and what was something that I found interesting or useful. Keep in mind I already have some knowledge
of Unreal Engine and I have extensive knowledge in C++ Programming.

## The Basics Part 1:
It is interesting for me the dynamic between C++ and Blueprint, although we currently create
objects from scratch in C++, basic things like creating properties and assiging static meshes
are a lot faster in Blueprints. Also tweaking values and testing is much faster. However, if you
dig a little in the engine you can find very good information and documetation inside the source code
itself. There is a very good documenation on the enum for the UPROPERTY, UCLASS, etc in a ObjectMacros.h
file. So yeah you just have to dig the engine a little bit and coding in C++ becomes easy. Something
to adjust to is the type system. They have their own types for basically everything so its like re-learning
C++ in a way because you have to understand their APIs and how to use them.

### Key take aways
1. Do not trust hot-reload it can crash or does not properly change blueprint values.
2. Character movement is essential, spend a good time desiging it and then implementing it.
	- Always try to find something that already solves you problem most likely the engine has something for you.


## The Basics Part 2:
Distinction between types of components, actors, and other things. For the longest time I could not understand
what is the difference between UStaticMesh and UStaticMeshComponent or basically anything with and without component.
Well basically if it is without the component you are reffering to the actual asset not to the component that you attach.
So yeah thats something new. Also there is a slight convention when it comes to attachable and non-attachable components like
UMovementComponent. It is a bit hard to know which is which so the best way is to visualize it in the blueprint if you ahve any doubt.

The major lessons here are the suffisticated delegate and message passing system in unreal engine. I think the event system as a whole is brilliant
in the engine and I have only scratched the surface. it is easy to use and very expressive. Should be used for many many things. One thing
I found that helps a lot if digging in the engine architecture and understanding the lifecycle of objects, how the engine starts and runs and
what is going on each frame. This allows you to very easily add new functionality at the right stage of an object. For example there is a function called
PostInitializeComponent which is ran after the constructor of an actor once the components are assembled. In this function is a great place
to add dynamic delegates for some events. However, in order to know this you kind of have to know the architecture of the engine itself and what is going
on with the object lifecycle.

Another major thing is the reflection system and the unreal header tool. I think that in order to understand fully how the C++ works you need to
know the header tool and the property system. I was trying to create and interface and it took me way too long because I did not read the documentation
inside the engine. in order to create an interface first you need reflection, then you also need another normal class, and then you need to know which is which.
THe interface is always with U, however there is nothing in this class nothiiiing it is pure reflection. Then we have a normal I class with the same name,
there we define the functions that we want and we tag them with UFUNCTION to make them either native only or blueprint callable or implementable. Then
we can inherit from this I class in an actor and implement the method buuuuut there is a caviat the implementation needs to be in the form:
InterfaceFunctionName_Implementation(....); Then how do we use this iinterface we can do something like: Character->GetClass()->ImplementsInterface(..)
HOLD on here is the caviat on the I class but the U class should be the argument. I think by this point you are either very confused or have stopped following actively
the point is it is very messy and you need to get used to it and practice.


## The Basics Input Systems

I was used to the old system, however, Epic Added a new one, which although it is a bit tricky to learn at first provides so mcuh more flexibility.
Basically you have these input mapping contentx, a player can have multiple of them for diferent situations for example, when swimming, when in vechicle, or normal.
Each mapping context defines a sets of actions for specific inputs. So for my case for now I need one mapping context and I want to have the following capabilities:

	1. Move
	2. Jump
	3. Primary Attack
	4. Look
	5. Interact with items

For moving we need again a function to bind,however now we also need to create an action asset. Then we need to bind it to the player blueprint. This action asset
will define the 4 keys: W,A,S,D for moving allong with the modifiers and trigger. This is again something new. Basically a modifier explain how to modify the raw input
for example we can negate it. If we press S we want to move backwards so we negate it. We can swizzle it which basically means swap the AXIS values. And many more common ones. We can also create our own if neede but I wil not do it now. Next we have the triggers. This can mean stuff like: pressing, holding, tapping, combos, etc. Again
super versitile, most of the time you do not need to assign them, if you need it very simple. Now that we have the action in C++ we need to define a function
called for example move than will take as a parameter FInputActionValue const &. This action will store the value of the input and based on it we can define our
own logic. Sometimes you do not need to use the value, however, you still need to place it in the function arguments. So yeah this is it for the new input system.

## Sweeping and Hitting
I guess it helps to know the Engine architecutre, but for me ones I realized that the world holds all the context everything seemed to become very intuitive. Want to spawn
an actor? use the world. Want to spawn an emitter? use the world, etc, etc, etc. So for sweeping we do the same. It is again made very intuitive. First you have many options
for sweeping, by objects, tags, channels, etc. And then the actual calls are very easy you just fill out a struct nad call the method. I really enjoyt this method of programming because it is very declarative like Vulkan API. Overall just find what you want to sweep, fill a struct, get the hits, in an array if you have many, then
you can do whatever you want with them. in my case i trigger the implementation of the interaction interface for the closest object. However in some cases you can trigger
it for all objects if needed depending on the interface.

## Rotation, Raycasting, Projectile

So I decided to make 3 types of projectiles 2 in C++ and 1 in blueprint. Since all of them share common functionality I wanted to create a base class in C++ to reuse.
So again the little quircks of unreal engine. First I need to declare it as abstract using the UCLASS macro. Then I need to sepcify it is bluprint implementable. Finally
I can place the logic inside. An interesting thing I found was that I can do something like
	UFUNCTION()
	virtual void OnActorHit(Params...)
I though I could not do that for some reasons since it is an event however I actually can. The functionality is not important but the mechanism was interesting. Then I was able to create a child class from c++ and blueprint also. I wanted to fix my aiming to aim in the center of the screen which was actually not so easy. So first
you need to cast a ray from the camera to the center of the screen. There are different ways to do it and all of them feel weird to me. First I tried to deproject the
screen center into a point and cast to it which was not that nice. Then I was like okay lets just cast from the camera forward since this will be the center of the screen.
Well I need the camera rotation and location. However I cannot get them from the camera component like I can from any other component. You need to get the player controller
then get the camera manager and then do what you want to do. The casting itself is simple and after you cast you get the end point and calculate the rotation. Okay but how.

Option A: Use the Kismet Math library, however you have to include the header which is not optimal for one little function.
Option B: FRotationMatrix::MakeFromX(TraceEnd - TraceStart).Vector() Better!!
The quirk again is to remember which axis you need so you can get the proper rotation.

## Custom Attribute Component, Anim BPs and Widgets

Imagine you want to add health to the character. You can just add a field, but that becomes unscalable since for example enemies will also need the, you might have multiple
other entities which health, stamina, etc. So you create a custom actor component to store the health. Then I got to use the cool event system of unreal. Basically you declare a delegate with however many params needed. Then you create a function like ApplyHealthChange, change the health and broadcast through the delegate the change.
You can subscribe to this event via the component itself if you use it on anything. This makes it very easy to use event driven programming. This is a key concept which I explored in depth.

Events in widgets:(HEALTH BAR)
I created a material for the widget to display the progress (yes I know there is a progress bar for widgtes). Then on widget construction I subscribe to the on health changed
event with a custom event. Which will update the material. In this way we do not constantly tick but rather act on events.

Events in c++:
You basically use the existing events and make the code more event driven, should avoid using the tick function as much as possible.

Anim BP:
In the anim bp we again use the even functionality to inform if the player has died, if so then simply change play proper animation. Again events are key for this.

Widgets in general:

Very powerful and well intergated. Could be used for both user widget like health bars and also 3D widgest on the screen for items, damage, etc. Could nest widgets
and also apply materials to them.

Core Ideas and yet to explore:
	1. Use events as much as possible
	2. Heavy lifting in C++ only style and data change in blueprints
	3. Use time in materials
	4. Explore the damage system
	5. Explore animation further
	6. Explore anim notifies
	7. Explore the different blending spaced
	8. Explore more on materials and how to create interesting effects, learn the difference between when to sue material and niagra
	9. Explore niagra and its capabilities

## Animating widgets and projecting on screen
Widgets can be super easily animated which allows for some nice moves. For example I have a damage widget that spawns when you damage an actor. This widget get animated
by size and color. I again use the "expose on spawn" for variables so that I can assign them properly when needed. For projecting the widget I just project a point in 3D
space to a screen space point and position the widget there. An important note is that I currently do not use the built in damage system by epic which is something that I should definitely pick up on doing.

Another animation which I have added to the projectile and character is the camera shake. When the projectile hits something or explodes, I create a camera shake. I have
also added a casting particle effect in the player hand. For this I just used the UGameplayStatics::SpawnEmitterAttached and attached it to a socket in the mesh.



