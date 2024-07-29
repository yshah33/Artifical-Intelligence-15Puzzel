#CS 411 - Assignment 5 Solution
#Astar Search on 15 Puzzle
#Aniket Anil Wagde
#2022 Fall

import random
import math
import time
import psutil
import os
from collections import deque
from heapq import *
import sys

#This class defines the state of the problem in terms of board configuration
class Board:
	def __init__(self,tiles):
		self.size = int(math.sqrt(len(tiles))) # defining length/width of the board
		self.tiles = tiles
	
	#This function returns the resulting state from taking particular action from current state
	def execute_action(self,action):
		new_tiles = self.tiles[:]
		empty_index = new_tiles.index('0')
		if action=='l':	
			if empty_index%self.size>0:
				new_tiles[empty_index-1],new_tiles[empty_index] = new_tiles[empty_index],new_tiles[empty_index-1]
		if action=='r':
			if empty_index%self.size<(self.size-1): 	
				new_tiles[empty_index+1],new_tiles[empty_index] = new_tiles[empty_index],new_tiles[empty_index+1]
		if action=='u':
			if empty_index-self.size>=0:
				new_tiles[empty_index-self.size],new_tiles[empty_index] = new_tiles[empty_index],new_tiles[empty_index-self.size]
		if action=='d':
			if empty_index+self.size < self.size*self.size:
				new_tiles[empty_index+self.size],new_tiles[empty_index] = new_tiles[empty_index],new_tiles[empty_index+self.size]
		return Board(new_tiles)
		

#This class defines the node on the search tree, consisting of state, parent and previous action		
class Node:
	def __init__(self,state,parent,action):
		self.state = state
		self.parent = parent
		self.action = action
		if parent is None:
			self.cost=0
		else:
			self.cost = parent.cost+1
	
	#Returns string representation of the state	
	def __repr__(self):
		return str("state: " +str(self.state.tiles) + "prev_action "+ str(self.action))
	
	#Comparing current node with other node. They are equal if states are equal	
	def __eq__(self,other):
		return self.state.tiles == other.state.tiles
		
	def __lt__(self, other):
		return id(self)<= id(other)
	
	def __hash__(self):
		return hash(tuple(self.state.tiles))

#This class defines astar search problem		
class Astar:
	def __init__(self,start,goal,heuristic):
		self.root_node = start
		self.goal_state = goal
		self.heuristic = heuristic
		self.node_expanded = 0
		
	def run(self):
		frontier = []
		explored = set()
		max_memory = 0
		heappush(frontier, (self.f_value(self.root_node), self.root_node))
		expanded_node_count=0
		while(len(frontier)>0):
			max_memory = max(max_memory, sys.getsizeof(frontier)+sys.getsizeof(explored))	
			cur_node = heappop(frontier)[1] # we only need node , not the cost
			expanded_node_count+=1
			explored.add(cur_node)
			if(self.goal_test(cur_node.state.tiles)):
				path = find_path(cur_node)
				return path,expanded_node_count, max_memory
			for child in get_children(cur_node):
				if child in explored:
					continue
				else:
					heappush(frontier, (self.f_value(child), child))
		print("frontier empty")	
		return False
	
	def f_value(self,node):
		return node.cost + self.h_value(node)
		
	def h_value(self,node):
		if self.heuristic=="manhattan":
			return self.manhattan_heuristic(node)
		else:
			return self.misplaced_tiles_heuristic(node)
	
	#This function calculates sum of manhattan distances of each tile from goal position
	def manhattan_heuristic(self,node):
		tiles = node.state.tiles
		size = node.state.size
		total_sum_distances = 0 
		for i in range(0,len(tiles)):
			value = int(tiles[i])
			if value==0 : continue
			current_x = i / size
			current_y = i % size
			correct_x = (value-1) / size
			correct_y = (value-1) % size
			
			
			cur_distance = abs(correct_x-current_x) + abs(correct_y-current_y)
			total_sum_distances += cur_distance
		return total_sum_distances
	
	#This function calculates number of misplaced tiles from goal position
	def misplaced_tiles_heuristic(self,node):
		tiles = node.state.tiles
		num_misplaced = 0
		for i in range(1,len(tiles)):
			if i!=int(tiles[i-1]) : num_misplaced+=1
			
		return num_misplaced
		
	#Utility function checking if current state is goal state or not
	def goal_test(self,cur_state):
		return cur_state == self.goal_state
	
		
#Utility function to randomly generate 15-puzzle		
def generate_puzzle(size):
	numbers = list(range(size*size))
	random.shuffle(numbers)
	return Node(Board(numbers),None,None)

#This function returns the list of children obtained after simulating the actions on current node
def get_children(parent_node):
	children = []
	actions = ['l','r','u','d'] # left,right, up , down ; actions define direction of movement of empty tile
	for action in actions:
		child_state = parent_node.state.execute_action(action)
		child_node = Node(child_state,parent_node,action)
		children.append(child_node)
	return children

#This function backtracks from current node to reach initial configuration. The list of actions would constitute a solution path
def find_path(node):	
	path = []	
	while(node.parent is not None):
		path.append(node.action)
		node = node.parent
	path.reverse()
	return path



	
#Main function accepting input from console , runnung bfs and showing output	
def main():
	process = psutil.Process(os.getpid())
	initial_memory = process.memory_info().rss / 1024.0
	initial_time = time.time()
	initial = str(input("initial configuration: "))
	initial_list = initial.split(" ")
	root = Node(Board(initial_list),None,None)
	goal_state = ['1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','0']	
	
	#astar with manhattan distance heurisitcs
	astar = Astar(root, goal_state,"manhattan")
	print("manhattan_heuristic: "+ str(astar.manhattan_heuristic(root)))
	
	
	solution = astar.run()
	final_time = time.time()
	print("path: "+ str(solution[0]))
	print("nodes_expanded:" + str(solution[1]))
	#final_memory = process.memory_info().rss / 1024.0
	print("time taken: "+str(final_time-initial_time))
	#print(str(final_memory-initial_memory)+" KB")
	print("maximum memory: "+str(solution[2])+" KB")
	
	#astar with misplaced tiles heurisitcs
	astar = Astar(root, goal_state,"misplaced")
	print("misplaced_tiles_heuristic: "+ str(astar.misplaced_tiles_heuristic(root)))
	
	solution = astar.run()
	final_time = time.time()
	print("path: "+ str(solution[0]))
	print("nodes_expanded:" + str(solution[1]))
	#final_memory = process.memory_info().rss / 1024.0
	print("time taken: "+str(final_time-initial_time))
	#print(str(final_memory-initial_memory)+" KB")
	print("maximum memory: "+str(solution[2])+" KB")
	
if __name__=="__main__":main()	