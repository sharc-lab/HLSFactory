# External imports
import numpy as np
from typing import Union

from disjoint_set import DisjointSet
import dataclasses
from scipy.stats import mode
import glob
import os

@dataclasses.dataclass
class EventInfo:
    n_pixels: Union[np.ndarray]
    energy: Union[np.ndarray]
    momentum: Union[np.ndarray]
    interaction_point: Union[np.ndarray]
    trigger: Union[bool]
    has_trigger_pair: Union[bool]
    track_origin: Union[np.ndarray]
    trigger_node: Union[np.ndarray]
    particle_id: Union[np.ndarray]
    particle_type: Union[np.ndarray]
    parent_particle_type: Union[np.ndarray]
    track_hits: Union[np.ndarray]
    track_n_hits: Union[np.ndarray]

def get_tracks_ds(edge_index):
    # Get connected components
    ds = DisjointSet()
    for i in range(edge_index.shape[1]):
        ds.union(edge_index[0, i], edge_index[1, i])

    return tuple(list(x) for x in ds.itersets())

def get_tracks(edge_index):
    if edge_index.shape[1] == 0:
        return []

    adj_list = [[] for i in range(np.max(edge_index)+1)]
    for (start, finish) in edge_index.T:
        adj_list[start].append(finish)
        adj_list[finish].append(start)

    visited = np.zeros(np.max(edge_index)+1)
    components = []

    
    component = []
    def dfs(root):
        if visited[root]:
            return

        visited[root] = 1
        component.append(root)
        for neighbor in adj_list[root]:
            dfs(neighbor)

    for i in range(len(visited)):
        if not visited[i]:
            dfs(i)
            components.append(component)
            component = []

    return tuple(components)
            

# The actual meet of how a graph is constructed
def build_graph(filename, min_edge_probability=0.5, intt_required=False):
    layers = [(0,), (1,), (2,), (3,4), (5,6)]
    with np.load(filename) as f:
        model_edge_probability = f['model_edge_probability']
        edge_index = f['edge_index'][:, model_edge_probability >= min_edge_probability]
        tracks = get_tracks(edge_index)
        if intt_required:
            tracks = [track for track in tracks if np.any(f['layer_id'][track] >= 3)]

        track_hits = np.zeros((len(tracks), 3*len(layers)))
        n_pixels = np.zeros((len(tracks), len(layers)))
        energy = np.zeros(len(tracks))
        momentum = np.zeros((len(tracks), 3))
        track_origin = np.zeros((len(tracks), 3))
        trigger_node = np.zeros(len(tracks))
        particle_id = np.zeros(len(tracks))
        particle_type = np.zeros(len(tracks))
        parent_particle_type = np.zeros(len(tracks))
        track_n_hits = np.zeros((len(tracks), len(layers)))

        for i, track in enumerate(tracks):
            layer_id = f['layer_id'][track]
            hit_n_pixels = f['n_pixels'][track]
            hits = f['hit_cartesian'][track]

            # Calculate per-layer information
            for j, layer in enumerate(layers):
                mask = np.isin(layer_id, layer)
                weighted_hits = hit_n_pixels[mask, None] * hits[mask]
                d = np.sum(hit_n_pixels[mask])

                track_hits[i, 3*j:3*(j+1)] = np.sum(weighted_hits, axis=0)/(d + (d == 0))
                n_pixels[i, j] = d
                track_n_hits[i, j] = np.sum(mask)
            
            # Find the GT particle that this track is assigned to
            pids = f['particle_id'][track]
            particle_id[i] = mode(pids, axis=0, keepdims=False).mode
            if np.isnan(particle_id[i]):
                index = track[np.where(np.isnan(pids))[0][0]]
            else:
                index = track[np.where(pids == particle_id[i])[0][0]]

            energy[i] = f['energy'][index]
            momentum[i] = f['momentum'][index]
            track_origin[i] = f['track_origin'][index]
            trigger_node[i] = f['trigger_node'][index]
            particle_type[i] = f['particle_type'][index]
            parent_particle_type[i] = f['parent_particle_type'][index]

        event_info = EventInfo(
                n_pixels=n_pixels,
                energy=energy,
                momentum=momentum,
                interaction_point=f['interaction_point'],
                trigger=f['trigger'],
                has_trigger_pair=f['has_trigger_pair'],
                track_origin=track_origin,
                trigger_node=trigger_node,
                particle_id=particle_id,
                particle_type=particle_type,
                parent_particle_type=parent_particle_type,
                track_hits=track_hits,
                track_n_hits=track_n_hits
        )

        return event_info

INPUT_DIRS = ['input_graphs/trigger/1/', 'input_graphs/nontrigger/0/']
OUTPUT_DIRS = ['outputs/trigger/', 'outputs/nontrigger/']

def main():
    input_file_sets = [glob.glob(os.path.join(dir, '*.npz')) for dir in INPUT_DIRS]

    def process(file, output_dir):
        event_info = build_graph(file)
        event_info_dict = dataclasses.asdict(event_info)
        np.savez(os.path.join(output_dir, os.path.basename(file)), **event_info_dict)


    for input_file_set, output_dir in zip(input_file_sets, OUTPUT_DIRS):
        cnt = 0
        os.makedirs(output_dir, exist_ok=True)
        for filename in input_file_set:
            process(filename, output_dir)
            cnt = cnt + 1
            print("Processed " + str(cnt) + "-th graph")


if __name__ == '__main__':
    main()
