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

