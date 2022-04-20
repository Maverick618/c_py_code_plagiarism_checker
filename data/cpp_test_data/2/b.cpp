class RandomizedSet {
private:
    vector<int> nums;
    unordered_map<int, int> indices;
public:
    RandomizedSet() {
        srand((unsigned)time(NULL));
    }
    int getRandom() {
        int randomIndex = rand()%nums.size();
        return nums[randomIndex];
    }
    bool remove(int val) {
        if (!indices.count(val)) {
            return false;
        }
        int index = indices[val];
        int last = nums.back();
        nums[index] = last;
        indices[last] = index;
        nums.pop_back();
        indices.erase(val);
        return true;
    }
    bool insert(int val) {
        if (indices.count(val)) {
            return false;
        }
        int index = nums.size();
        nums.emplace_back(val);
        indices[val] = index;
        return true;
    }
};
