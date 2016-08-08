#include <iostream>
#include <fstream>
#include <chrono>
#include <exception>
#include <numeric>
#include <random>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>

// No-null comment: 1

const double eps = 1.e-2;

template <class T>
class MathVect;

template <class T>
std::ostream& operator<<(std::ostream& out, MathVect<T> m);

template <class T = double>
class MathVect : public std::vector<T> {
public:
	MathVect(size_t size = 0) : std::vector<T>(size, T(0)) { }
	MathVect(size_t size, T d) : std::vector<T>(size, d) { }
	void resize(size_t s) {
		std::vector<T>::resize(s, 0);
	}
	MathVect operator * (double lambda) {
		MathVect res = *this;
		for (auto & e: res) {
			e = e * lambda;
		}
		return res;
	}
	auto operator * (const MathVect& other) -> decltype((*this)[0] * other[0]){
#ifdef DEBUG
		if (std::vector<T>::size() != other.size())
			throw std::logic_error("vectors for summing must be equal size");
#endif
		decltype((*this)[0] * other[0]) sum(0);
		for (int i = 0; i < std::vector<T>::size(); ++i)
			sum += (*this)[i] * other[i];
		return sum;
	}
	auto operator * (const MathVect&& other) -> decltype((*this)[0] * other[0]) {
#ifdef DEBUG
		if (std::vector<T>::size() != other.size())
			throw std::logic_error("vectors for summing must be equal size");
#endif
		decltype((*this)[0] * other[0]) sum(0);
		for (int i = 0; i < std::vector<T>::size(); ++i)
			sum += (*this)[i] * other[i];
		return sum;
	}
	T sum() {
		T res(0);
		for (const T& e: *this)
			res += e;
		return res;
	}
	MathVect operator + (MathVect& other) {
#ifdef DEBUG
		if (std::vector<T>::size() == 0) 
			resize(other.size());
		if (other.size() == 0)
			other.resize(std::vector<T>::size());
		if (std::vector<T>::size() != other.size())
			throw std::logic_error("vectors for summing must be equal or zero size");
#endif
		MathVect res(std::vector<T>::size());
		for (int i = 0; i < std::vector<T>::size(); ++i)
			res[i] = (*this)[i] + other[i];
		return res;	
	}
	MathVect operator + (MathVect&& other) {
#ifdef DEBUG
		if (std::vector<T>::size() == 0) 
			resize(other.size());
		if (other.size() == 0)
			other.resize(std::vector<T>::size());
		if (std::vector<T>::size() != other.size())
			throw std::logic_error("vectors for summing must be equal or zero size");
#endif
		MathVect res(std::vector<T>::size());
		for (int i = 0; i < std::vector<T>::size(); ++i)
			res[i] = (*this)[i] + other[i];
		return res;	
	}
	MathVect operator - (MathVect& other) {
#ifdef DEBUG
		if (std::vector<T>::size() == 0) 
			resize(other.size());
		if (other.size() == 0)
			other.resize(std::vector<T>::size());
		if (std::vector<T>::size() != other.size())
			throw std::logic_error("vectors for summing must be equal or zero size");
#endif
		MathVect res(std::vector<T>::size());
		for (int i = 0; i < std::vector<T>::size(); ++i)
			res[i] = (*this)[i] - other[i];
		return res;	
	}
	MathVect operator - (MathVect&& other) {
#ifdef DEBUG
		if (std::vector<T>::size() == 0) 
			resize(other.size());
		if (other.size() == 0)
			other.resize(std::vector<T>::size());
		if (std::vector<T>::size() != other.size())
			throw std::logic_error("vectors for summing must be equal or zero size");
#endif
		MathVect res(std::vector<T>::size());
		for (int i = 0; i < std::vector<T>::size(); ++i)
			res[i] = (*this)[i] - other[i];
		return res;	
	}
	void operator += (MathVect& other) {
		*this = *this + other;
	}
	void operator += (MathVect&& other) {
		*this = *this + other;
	}
};

MathVect<double> RandomVector(size_t size) {
	static std::random_device rd;
	static std::default_random_engine re(rd());
	static std::uniform_real_distribution<double> dist(0, 2);

	MathVect<double> res(size);
	for (auto & e: res)
		e = dist(re);
	return res;
}

template <class T>
std::ostream& operator<<(std::ostream& out, MathVect<T> m) {
	out << "(";
	for (int i = 0; i < m.size(); ++i)
		out << m[i] << ((i != (m.size() - 1)) ? ", " : "");
	out << ")";
	return out;
}

using Mat = MathVect<MathVect<double>>;

using UID = int;
using IID = int;

template <class Meta>
class PairDataSet
{
private:
	std::map<UID, std::set<IID>> user_map;
	std::map<IID, std::set<UID>> item_map;

public:
	std::map<std::pair<UID, IID>, Meta> data;

public:
	void AddData(UID uid, IID iid, Meta meta) {
		if (user_map.find(uid) == user_map.end())
			user_map.emplace(uid, std::set<IID> {});
		user_map[uid].insert(iid);
		if (item_map.find(iid) == item_map.end())
			item_map.emplace(iid, std::set<UID> {});
		item_map[iid].insert(iid);
		data[std::make_pair(uid, iid)] = meta;
	}
	
	auto BeginUser(UID uid) -> decltype(user_map.at(uid).begin()) {
		return user_map.at(uid).begin();
	}

	auto EndUser(UID uid) -> decltype(this->user_map.at(uid).end()) {
		return user_map.at(uid).end();
	}

	auto BeginItem(IID iid) -> decltype(this->item_map.at(iid).begin()) {
		return item_map.at(iid).begin();
	}

	auto EndItem(IID iid) -> decltype(this->item_map.at(iid).end()) {
		return item_map.at(iid).end();
	}
	
	Meta GetData(UID uid, IID iid) {
		return data.at(std::make_pair(uid, iid));
	}
};

template <class Meta>
class TrivialPairDataSet
{
public:
	struct Data {
		UID uid;
		IID iid;
		Meta meta;
	};
	std::vector<Data> data;

public:
	void AddData(UID uid, IID iid, Meta meta) {
		data.push_back({uid, iid, meta});
	}
};

using RecommederDataSet = PairDataSet<double>;
using RecommederTrivialDataSet = TrivialPairDataSet<double>;

class SVDRecommender 
{
public:
	std::vector<MathVect<>> users;
	std::vector<MathVect<>> items;
	
	SVDRecommender(int U = 0, int M = 0, int d = 3) : users(U, MathVect<>(d)), items(M, MathVect<>(d)) { }
	double Predict(int uid, int iid) 
	{
//		std::cout << "User: " << users[uid] << "\n" 
//			<< "Item: " << items[iid] << "\n"
//			<< "Prediction: " << users[uid] * items[iid] << "\n";
		return users[uid] * items[iid];
	}
};

class SVDRecommenderTrainerInterface
{
public:
	SVDRecommender* recommender;
	
	SVDRecommenderTrainerInterface(SVDRecommender* recommender)
		: recommender(recommender) 
	{ 
	}
	virtual void Train() = 0;
	virtual double RMSE() = 0;
};

class SVDRecommenderTrainerStochasticGradient : public SVDRecommenderTrainerInterface {
public: 
	SVDRecommenderTrainerStochasticGradient(SVDRecommender* recommender, RecommederDataSet* data)
		: SVDRecommenderTrainerInterface(recommender)
		, data(data) 
	{ 
	}
	void Train() override {
		RandomizeData();
//		std::cout << "First RMSE " << RMSE() << "\n";
		int user = 0;
		int item = 0;		
		double lambda = 0.01;
		for (int i = 0; i < N; ++i) {
			MakeUserStep(user, lambda);
			MakeItemStep(item, lambda);
			++user;
			++item;
			user %= recommender->users.size();
			item %= recommender->items.size();
			double rmse = RMSE();
			std::cout << rmse << "\n";
			if (rmse < eps)
			{
				std::cout << "Stop training at " << i << " iteration\n";
				break;
			}
		}
	}
	double RMSE() override {
		double sum = 0;
		for (const auto& d: data->data) {
//			std::cout << "RMSE: r = " << d.second << "; prediction = " << recommender->Predict(d.first.first, d.first.second) <<"\n";
			sum += std::pow(d.second - recommender->Predict(d.first.first, d.first.second), 2);
		}
		return sum;
	}
	int N = 3000;
private:
	RecommederDataSet* data;
	void MakeUserStep(UID uid, double lambda) {
		recommender->users[uid] += std::accumulate(data->BeginUser(uid), data->EndUser(uid), MathVect<>(),
			[&] (MathVect<> sum, IID iid) 
			{
				double k = (data->GetData(uid, iid) - recommender->users[uid] * recommender->items[iid]);
				MathVect<> delta = recommender->items[iid] * k;
				return sum + delta;
			}) * lambda;
	}
	void MakeItemStep(IID iid, double lambda) {
		recommender->items[iid] += std::accumulate(data->BeginItem(iid), data->EndItem(iid), MathVect<>(),
			[&] (MathVect<> sum, UID uid)
			{
				return sum + recommender->users[uid] * (data->GetData(uid, iid) - recommender->users[uid] * recommender->items[iid]);
			}) * lambda ;
	}
	void RandomizeData() {
		for (auto& user: recommender->users)
			user = RandomVector(user.size());
		for (auto& item: recommender->items)
			item = RandomVector(item.size());
	}
};

class SVDRecommenderTrainerStochasticGradientWithMuAndBias : public SVDRecommenderTrainerInterface {
public: 
	SVDRecommenderTrainerStochasticGradientWithMuAndBias(SVDRecommender* recommender, RecommederDataSet* data)
		: SVDRecommenderTrainerInterface(recommender)
		, data(data) 
	{ 
	}
	void Train() override {
		RandomizeData();
		EstimateMu();
		FixAll();
//		std::cout << "First RMSE " << RMSE() << "\n";
		int user = 0;
		int item = 0;		
		double lambda = 0.1;
		for (int i = 0; i < N; ++i) {
			double t_lambda = lambda * (1. - ((double)i * 9) / (10 * N));
			MakeUserStep(user, t_lambda);
			FixUser(user);
			MakeItemStep(item, t_lambda);
			FixItem(item);
			++user;
			++item;
			user %= recommender->users.size();
			item %= recommender->items.size();
			double rmse = RMSE();
//			std::cout << rmse << "\n";
			done_iterations = i;
			if (rmse < eps)
			{
//				std::cout << "Stop training at " << i << " iteration\n";
				break;
			}
		}
	}
	double RMSE() override {
		double sum = 0;
		for (const auto& d: data->data) {
//			std::cout << "RMSE: r = " << d.second << "; prediction = " << recommender->Predict(d.first.first, d.first.second) <<"\n";
			sum += std::pow(d.second - recommender->Predict(d.first.first, d.first.second), 2);
		}
		return sum;
	}
	int N = 5000;
	int done_iterations = 0;
private:
	RecommederDataSet* data;
	double mu;
	void MakeUserStep(UID uid, double lambda) {
		recommender->users[uid] += std::accumulate(data->BeginUser(uid), data->EndUser(uid), MathVect<>(),
			[&] (MathVect<> sum, IID iid) 
			{
				double k = (data->GetData(uid, iid) - recommender->users[uid] * recommender->items[iid]);
				MathVect<> delta = recommender->items[iid] * k;
				return sum + delta;
			}) * lambda;
	}
	void MakeItemStep(IID iid, double lambda) {
		recommender->items[iid] += std::accumulate(data->BeginItem(iid), data->EndItem(iid), MathVect<>(),
			[&] (MathVect<> sum, UID uid)
			{
				return sum + recommender->users[uid] * (data->GetData(uid, iid) - recommender->users[uid] * recommender->items[iid]);
			}) * lambda ;
	}
	void EstimateMu() {
		double sum = 0;
		for (const auto& d: data->data)
			sum += d.second;
		mu = sum / data->data.size();
//		std::cout << "Mu: " << mu << "\n";
	}

	// User: (mu,   1, b_u, ... )
	// ItemL ( 1, b_i,   1, ... )
	void FixUser(UID uid) {
		auto& user = recommender->users[uid];
		user[0] = mu;
		user[1] = 1.;
	}
	void FixItem(IID iid) {
		auto& item = recommender->items[iid];
		item[0] = 1.;
		item[2] = 1.;
	}
	void FixAll() {
		for (int i = 0; i < recommender->users.size(); ++i)
			FixUser(i);
		for (int i = 0; i < recommender->items.size(); ++i)
			FixItem(i);
	}
	void RandomizeData() {
		for (auto& user: recommender->users)
			user = RandomVector(user.size());
		for (auto& item: recommender->items)
			item = RandomVector(item.size());
	}
};

class SVDRecommenderOnlineTrainer: public SVDRecommenderTrainerInterface {
public:
	SVDRecommenderOnlineTrainer(SVDRecommender* recommender, std::istream& data_stream)
		: SVDRecommenderTrainerInterface(recommender)
		, timeup(5000)
		, data_stream(data_stream)
	{
	}

	void Train() override {
		RandomizeData();
		EstimateMu();
		FixAll();
//		std::cout << "First RMSE " << RMSE() << "\n";
		double lambda = 0.1;
		auto beginning = std::chrono::system_clock::now();
		for (int i = 0; i < N && std::chrono::system_clock::now() - beginning < timeup; ++i) {
			double t_lambda = lambda * (1. - ((double)i * 9) / (10 * N));
			
			data_stream.clear();
			data_stream.seekg(0);
			int k, U, M, D, T;
			data_stream >> k >> U >> M >> D >> T;
			for (int i = 0; i < D; ++i) {
				int u, m, r;
				data_stream >> u >> m >> r;
				
				double c = r - recommender->users[u] * recommender->items[m];
				c *= t_lambda;
				recommender->users[u] += recommender->items[m] * c; 
				FixUser(u);
				recommender->items[m] += recommender->users[u] * c;
				FixItem(m);
			}

//			double rmse = RMSE();
//			std::cout << rmse << "\n";
			done_iterations = i;
//			if (rmse < eps)
//			{
//				std::cout << "Stop training at " << i << " iteration\n";
//				break;
//			}
		}
	}

	double RMSE() override {
		double sum = 0;
		data_stream.clear();
		data_stream.seekg(0);
		int k, U, M, D, T;
		data_stream >> k >> U >> M >> D >> T;
		for (int i = 0; i < D; ++i) {
			int u, m, r;
			data_stream >> u >> m >> r;
			sum += std::pow(r - recommender->Predict(u, m), 2);
		}
		return sum;
	}
public:
	std::chrono::milliseconds timeup;
	int N = 1000;
	int done_iterations = 0;
private:
	std::istream& data_stream;
	double mu;
private:
	void EstimateMu() {
		double sum = 0;
		data_stream.clear();
		data_stream.seekg(0);
		int k, U, M, D, T;
		data_stream >> k >> U >> M >> D >> T;
		for (int i = 0; i < D; ++i) {
			int u, m, r;
			data_stream >> u >> m >> r;
			sum += r; 
		}
		mu = sum / D;
//		std::cout << "Mu: " << mu << "\n";
	}
	// User: (mu,   1, b_u, ... )
	// ItemL ( 1, b_i,   1, ... )
	void FixUser(UID uid) {
		auto& user = recommender->users[uid];
		user[0] = mu;
		user[1] = 1.;
	}
	void FixItem(IID iid) {
		auto& item = recommender->items[iid];
		item[0] = 1.;
		item[2] = 1.;
	}
	void FixAll() {
		for (int i = 0; i < recommender->users.size(); ++i)
			FixUser(i);
		for (int i = 0; i < recommender->items.size(); ++i)
			FixItem(i);
	}
	void RandomizeData() {
		for (auto& user: recommender->users)
			user = RandomVector(user.size());
		for (auto& item: recommender->items)
			item = RandomVector(item.size());
	}
};

class Shuffler {
private:
	std::vector<int> index;
public:
	Shuffler(int n)
		: index(n)
	{
		for (int i = 0; i < index.size(); ++i)
			index[i] = i;
		std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(index.begin(), index.end(), g);
	}
	int operator() (int i) {
		return index.at(i);
	}
};

class SVDRecommenderSemiOnlineTrainer: public SVDRecommenderTrainerInterface {
public:
	SVDRecommenderSemiOnlineTrainer(
			  SVDRecommender* recommender
			, RecommederTrivialDataSet* data
			, int timeup
			, double lambda
			, double regularization_k
			, long long N = 1000)
		: SVDRecommenderTrainerInterface(recommender)
		, timeup(timeup) 
		, N(N)
		, lambda(lambda) 
		, regularization_k(regularization_k)
		, data(data)
		, shuffle(data->data.size())
	{
	}

	void Train() override {
		RandomizeData();
		EstimateMu();
		FixAll();
		recommender_backup = *recommender;
		int rmse_offset = 0;
		double rmse = RMSE_partial(rmse_offset);
		double speed = -1.0;
		double acceleration = 0.0;
		double last_lambda = 0;
		std::cerr << "First RMSE: " << rmse << "\n";
		std::ofstream lambda_log("lambda.log");
		std::ofstream rmse_log("rmse.log");
		std::ofstream speed_log("speed.log");
		std::ofstream acceleration_log("acceleration.log");
		long long log_id = 0;
		auto beginning = std::chrono::system_clock::now();
			
		double max = 0;

		for (int i = 0; i < N && std::chrono::system_clock::now() - beginning < timeup; ++i) {
			double t_lambda = lambda;
			const auto& d = data->data[shuffle(i % data->data.size())];
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;

			double c = r - recommender->users[u] * recommender->items[m];
			c *= t_lambda;
			recommender->users[u] += (recommender->items[m] 
				- recommender->users[u] * regularization_k) * c; 
			FixUser(u);
			recommender->items[m] += (recommender->users[u]
				- recommender->items[m] * regularization_k) * c; 
			FixItem(m);
/*			
			for (int i = 2; i < recommender->users[u].size(); ++i) {
				const auto& user_i = recommender->users[u][i];
				max = std::max(max, std::abs(user_i));
				if (std::isnan(user_i))
					max = 1e100;
			}
			
			for (int i = 3; i < recommender->items[m].size(); ++i) {
				const auto& item_i = recommender->items[m][i];
				max = std::max(max, std::abs(item_i));
				if (std::isnan(item_i))
					max = 1e100;
			}
			max = std::max(max, std::abs(recommender->items[m][1]));
			if (std::isnan(recommender->items[m][1]))
				max = 1e100;
*/
//			double rmse = RMSE();
//			std::cerr << rmse << "\n";
			done_iterations = i + 1;
//			if (rmse < eps)
//			{
//				std::cout << "Stop training at " << i << " iteration\n";
//				break;
//			}
			if (done_iterations % 200000 == 0) {
#ifdef DEBUG
				std::cerr << "LogID: " << log_id << "\n";
#endif
				++log_id;
				lambda_log << lambda << "\n";
				rmse_log << rmse << "\n";
				speed_log << speed << "\n";
				acceleration_log << acceleration << "\n";
#ifdef DEBUG
				std::cerr << "RMSE (partial): " << rmse <<"\n";
#endif

				double lambda_backup = lambda;
				lambda_backup /= 1.1;

				if (max > 100.) {
					lambda /= 2;
#ifdef DEBUG
					std::cerr << "Max-dec lambda. New lambda: " << lambda << "\n";
#endif
				}
//				double new_rmse = RMSE_partial(rmse_offset);
				double new_rmse = RMSE();
				double new_speed = rmse - new_rmse;
				double new_acceleration = new_speed - speed;
				double new_lambda = lambda;
				double adjusted_acceleration = new_speed - speed * (lambda / last_lambda);

#ifdef DEBUG
				std::cerr << "New rmse: " << new_rmse << "\n";
				std::cerr << "New speed: " << new_speed << "\n";
				std::cerr << "New acceleration: " << new_acceleration << "\n";
				std::cerr << "Adj acceleration: " << adjusted_acceleration << "\n";
#endif
				/*
				if (new_speed < 0.01 * speed || new_speed < 0.) {
					new_lambda /= 1.1;
					if (new_speed < -0.1) {
						new_lambda /= 2;
						std::cerr << "Speed-dec-hard lambda. New lambda: " << new_lambda << "\n";
					}
					std::cerr << "Speed-dec lambda. New lambda: " << new_lambda << "\n";
				} else {
					new_lambda *= 1.1;
					std::cerr << "Speed-inc lambda. New lambda: " << new_lambda << "\n";
				}
				*/
				if (new_acceleration * adjusted_acceleration < 0) {
					std::cerr << "Adjusted has another sign\n";
				}

				if (acceleration > 0 && new_speed > 0) {
					new_lambda *= 1.005;
				}
				if ((acceleration > 0 && new_speed > -1.) 
					|| (new_acceleration > -.5 && new_speed > 0)) {
					new_lambda *= 1.005;
#ifdef DEBUG
					std::cerr << "Speed-inc lambda. New lambda: " << new_lambda << "\n";
#endif
				}

				if (acceleration < -.5) {
					new_lambda /= 1.005;
#ifdef DEBUG
					std::cerr << "Speed-dec lambda. New lambda: " << new_lambda << "\n";
#endif
				}
				if (new_speed < -0.5) {
					new_lambda /= 2;
#ifdef DEBUG
					std::cerr << "Speed-dec-hard lambda. New lambda: " << new_lambda << "\n";
#endif
				}

//				if (new_speed < 0) {
//					*recommender = recommender_backup;
#ifdef DEBUG
//					std::cerr << "Reset with backup. speed < 0\n";
#endif
//				}

				if (new_speed < -100 
					|| std::isnan(new_speed) 
					|| std::isnan(new_acceleration) 
					|| std::isnan(new_rmse)) {
					*recommender = recommender_backup;
					new_lambda *= 0.8;
#ifdef DEBUG
					std::cerr << "Reset with backup.\n";
#endif
				} else {
					recommender_backup = *recommender;
					speed = new_speed;
					rmse_offset += 1000;
					rmse_offset %= data->data.size();
				//	rmse = RMSE_partial(rmse_offset);
					rmse = new_rmse;
					acceleration = new_acceleration;

					new_lambda = lambda;

					last_lambda = lambda;
				}
				lambda = lambda_backup;
				max = 0;
#ifdef DEBUG
				std::cerr << "----\n";
#endif
			}
		}
	}

	double RMSE() override {
		double sum = 0;
		for (const auto& d: data->data) {
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;
			double error = r - recommender->Predict(u, m);
			sum += std::pow(error, 2);
		}
		return sum / data->data.size();
	}

private:
	inline double RMSE_partial(int offset) {
		double sum = 0;
		int size = data->data.size();
		int part_size = std::min (10000, size);
		for (int i = 0; i < part_size; ++i) {
			auto const& d = data->data[shuffle((i + offset) % data->data.size())];	
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;
			sum += std::pow(r - recommender->Predict(u, m), 2);
		}
		return sum / part_size;
	}
public:
	std::chrono::milliseconds timeup;
	long long N = 1000;
	int done_iterations = 0;
	double lambda;
	double regularization_k;
private:
	RecommederTrivialDataSet* data;
	SVDRecommender recommender_backup;
	Shuffler shuffle;	
	double mu;
private:
	void EstimateMu() {
		double sum = 0;
		for (const auto& d: data->data) {
			int r = d.meta;
			sum += r; 
		}
		mu = sum / data->data.size();
//		std::cerr << "Mu: " << mu << "\n";
	}
	// User: (mu,   1, b_u, ... )
	// ItemL ( 1, b_i,   1, ... )
	inline void FixUser(UID uid) {
		auto& user = recommender->users[uid];
		user[0] = mu;
		user[1] = 1.;
	}
	inline void FixItem(IID iid) {
		auto& item = recommender->items[iid];
		item[0] = 1.;
		item[2] = 1.;
	}
	inline void FixAll() {
		for (int i = 0; i < recommender->users.size(); ++i)
			FixUser(i);
		for (int i = 0; i < recommender->items.size(); ++i)
			FixItem(i);
	}
	void RandomizeData() {
		for (auto& user: recommender->users)
			user = RandomVector(user.size());
		for (auto& item: recommender->items)
			item = RandomVector(item.size());
	}
};

class SVDRecommenderTrainerWithConsistentData: public SVDRecommenderTrainerInterface {
public:
	SVDRecommenderTrainerWithConsistentData(
			  SVDRecommender* recommender
			, RecommederTrivialDataSet* data
			, int timeup
			, double lambda
			, double regularization_k
			, long long N = 1000)
		: SVDRecommenderTrainerInterface(recommender)
		, timeup(timeup) 
		, N(N)
		, lambda(lambda) 
		, regularization_k(regularization_k)
		, data(data)
		, shuffle(data->data.size())
	{
	}

	void Train() override {
		RandomizeData();
		EstimateMu();
		FixAll();
		recommender_last_copy = *recommender;
//		int rmse_offset = 0;
//		double rmse = RMSE_partial(rmse_offset);
		double rmse = RMSE();
		double speed = -1.0;
		double acceleration = 0.0;
		double last_lambda = 0;
		std::cerr << "First RMSE: " << rmse << "\n";
		std::ofstream lambda_log("lambda.log");
		std::ofstream rmse_log("rmse.log");
		std::ofstream speed_log("speed.log");
		std::ofstream acceleration_log("acceleration.log");
		long long log_id = 0;
		auto beginning = std::chrono::system_clock::now();
			
		double max = 0;

		for (int i = 0; i < N && std::chrono::system_clock::now() - beginning < timeup; ++i) {
			double t_lambda = lambda;
			const auto& d = data->data[shuffle(i % data->data.size())];
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;

			double c = r - recommender_last_copy.users[u] * recommender_last_copy.items[m];
			c *= t_lambda;
			recommender->users[u] += (recommender_last_copy.items[m] 
				- recommender_last_copy.users[u] * regularization_k) * c; 
			FixUser(u);
			recommender->items[m] += (recommender_last_copy.users[u]
				- recommender_last_copy.items[m] * regularization_k) * c; 
			FixItem(m);
//			double rmse = RMSE();
//			std::cerr << rmse << "\n";
			done_iterations = i + 1;
//			if (rmse < eps)
//			{
//				std::cout << "Stop training at " << i << " iteration\n";
//				break;
//			}
			if (done_iterations % (data->data.size() / 10) == 0) {
#ifdef DEBUG
				std::cerr << "LogID: " << log_id << "\n";
#endif
				++log_id;
				lambda_log << lambda << "\n";
				rmse_log << rmse << "\n";
				speed_log << speed << "\n";
				acceleration_log << acceleration << "\n";
#ifdef DEBUG
				std::cerr << "RMSE (partial): " << rmse <<"\n";
#endif

				double lambda_backup = lambda;
				lambda_backup /= 1.1;

				if (max > 100.) {
					lambda /= 2;
#ifdef DEBUG
					std::cerr << "Max-dec lambda. New lambda: " << lambda << "\n";
#endif
				}
//				double new_rmse = RMSE_partial(rmse_offset);
				double new_rmse = RMSE();
				double new_speed = rmse - new_rmse;
				double new_acceleration = new_speed - speed;
//				double new_lambda = lambda;
				double adjusted_acceleration = new_speed - speed * (lambda / last_lambda);

#ifdef DEBUG
				std::cerr << "New rmse: " << new_rmse << "\n";
				std::cerr << "New speed: " << new_speed << "\n";
				std::cerr << "New acceleration: " << new_acceleration << "\n";
				std::cerr << "Adj acceleration: " << adjusted_acceleration << "\n";
#endif


				lambda = lambda_backup;
				recommender_last_copy = *recommender;
#ifdef DEBUG
				std::cerr << "----\n";
#endif
			}
		}
	}

	double RMSE() override {
		double sum = 0;
		for (const auto& d: data->data) {
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;
			double error = r - recommender->Predict(u, m);
			sum += std::pow(error, 2);
		}
		return sum / data->data.size();
	}

public:
	std::chrono::milliseconds timeup;
	long long N = 1000;
	int done_iterations = 0;
	double lambda;
	double regularization_k;
private:
	RecommederTrivialDataSet* data;
	SVDRecommender recommender_last_copy;
	Shuffler shuffle;	
	double mu;
private:
	void EstimateMu() {
		double sum = 0;
		for (const auto& d: data->data) {
			int r = d.meta;
			sum += r; 
		}
		mu = sum / data->data.size();
//		std::cerr << "Mu: " << mu << "\n";
	}
	// User: (mu,   1, b_u, ... )
	// ItemL ( 1, b_i,   1, ... )
	inline void FixUser(UID uid) {
		auto& user = recommender->users[uid];
		user[0] = mu;
		user[1] = 1.;
	}
	inline void FixItem(IID iid) {
		auto& item = recommender->items[iid];
		item[0] = 1.;
		item[2] = 1.;
	}
	inline void FixAll() {
		for (int i = 0; i < recommender->users.size(); ++i)
			FixUser(i);
		for (int i = 0; i < recommender->items.size(); ++i)
			FixItem(i);
	}
	void RandomizeData() {
		for (auto& user: recommender->users)
			user = RandomVector(user.size());
		for (auto& item: recommender->items)
			item = RandomVector(item.size());
	}
};

class SVDRecommenderConjugateGradient: public SVDRecommenderTrainerInterface {
public:
	SVDRecommenderConjugateGradient(
			  SVDRecommender* recommender
			, RecommederTrivialDataSet* data
			, int timeup
			, double lambda
			, double regularization_k
			, long long N = 1000)
		: SVDRecommenderTrainerInterface(recommender)
		, timeup(timeup) 
		, N(N)
		, lambda(lambda) 
		, regularization_k(regularization_k)
		, data(data)
		, shuffle(data->data.size())
	{
	}

	void Train() override {
		RandomizeData();
		EstimateMu();
		FixAll();
		int rmse_offset = 0;
		double rmse = RMSE();
		double speed = -1.0;
		double acceleration = 0.0;
		double last_lambda = 0;
		std::cerr << "First RMSE: " << rmse << "\n";
		std::ofstream lambda_log("lambda.log");
		std::ofstream rmse_log("rmse.log");
		std::ofstream speed_log("speed.log");
		std::ofstream acceleration_log("acceleration.log");
		long long log_id = 0;
		auto beginning = std::chrono::system_clock::now();
			
		const int U = recommender->users.size();
		const int M = recommender->items.size();
		const int dim = recommender->users[0].size();

		Mat user_delta_x_prev(U, MathVect<>(dim));
		Mat movie_delta_x_prev(M, MathVect<>(dim));
		Mat u0 = user_delta_x_prev;
		Mat m0 = movie_delta_x_prev;
		for (int iteration = 0; iteration < N && std::chrono::system_clock::now() - beginning < timeup && rmse > 0.1; ++iteration) {
			if (iteration % 10 == 0) {
				std::cerr << "Iteration: " << iteration << "\n";
				rmse = RMSE();
				std::cerr << "RMSE: " << rmse << "\n";
			}

			if (iteration % 2 == 0) { // # train u_id
				Mat user_delta_x(U, MathVect<>(dim));
				for (const auto& d: data->data) {
					int u = d.uid;
					int m = d.iid;
					int r = d.meta;
					user_delta_x[u] += recommender->items[m] * (r - recommender->Predict(u, m));
				}
				for(int u = 0; u < U; ++u) {
					user_delta_x[u][0] = 0;
					user_delta_x[u][1] = 0;
				}
				double beta_pr = iteration < 2 ? 0 : (user_delta_x * (user_delta_x - user_delta_x_prev)) / (user_delta_x_prev * user_delta_x_prev);
				double beta = std::max(0., beta_pr);
				u0 = user_delta_x + (u0 * beta);
				double t_num = 0;
				double t_den = 0;
				for (const auto& d: data->data) {
					int u = d.uid;
					int m = d.iid;
					int r = d.meta;
					t_num += (r - recommender->Predict(u, m)) * (u0[u] * recommender->items[m]);
					t_den += std::pow(u0[u] * recommender->items[m], 2); 
				}
				double t = t_num / t_den;
				for(int u = 0; u < U; ++u) {
					recommender->users[u] += u0[u] * t;
				}
        
				user_delta_x_prev = user_delta_x;
			} else { // # train m_id
				Mat movie_delta_x(M, MathVect<>(dim));
				for (const auto& d: data->data) {
					int u = d.uid;
					int m = d.iid;
					int r = d.meta;
					movie_delta_x[m] += recommender->users[u] * (r - recommender->Predict(u, m));
				}
				for(int m = 0; m < M; ++m) {
					movie_delta_x[m][0] = 0;
					movie_delta_x[m][2] = 0;
				}
				double beta_pr = iteration < 2 ? 0 : (movie_delta_x * (movie_delta_x - movie_delta_x_prev)) / (movie_delta_x_prev * movie_delta_x_prev);
				double beta = std::max(0., beta_pr);
				m0 = movie_delta_x + m0 * beta;
				double t_num = 0;
				double t_den = 0;
				for (const auto& d: data->data) {
					int u = d.uid;
					int m = d.iid;
					int r = d.meta;
					t_num += (r - recommender->Predict(u, m)) * (m0[m] * recommender->users[u]);
					t_den += std::pow(m0[m] * recommender->users[u], 2); 
				}
				double t = t_num / t_den;
				for (int m = 0; m < M; ++m) {
					recommender->items[m] += m0[m] * t;
				}
        
				movie_delta_x_prev = movie_delta_x;
			}
			// FixAll();
			done_iterations = iteration + 1;
		}
	}

	double RMSE() override {
		double sum = 0;
		for (const auto& d: data->data) {
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;
			double error = r - recommender->Predict(u, m);
			sum += std::pow(error, 2);
		}
		return sum / data->data.size();
	}

private:
	inline double RMSE_partial(int offset) {
		double sum = 0;
		int size = data->data.size();
		int part_size = std::min (10000, size);
		for (int i = 0; i < part_size; ++i) {
			auto const& d = data->data[shuffle((i + offset) % data->data.size())];	
			int u = d.uid;
			int m = d.iid;
			int r = d.meta;
			sum += std::pow(r - recommender->Predict(u, m), 2);
		}
		return sum / part_size;
	}
public:
	std::chrono::milliseconds timeup;
	long long N = 1000;
	int done_iterations = 0;
	double lambda;
	double regularization_k;
private:
	RecommederTrivialDataSet* data;
	Shuffler shuffle;	
	double mu;
private:
	void EstimateMu() {
		double sum = 0;
		for (const auto& d: data->data) {
			int r = d.meta;
			sum += r; 
		}
		mu = sum / data->data.size();
//		std::cerr << "Mu: " << mu << "\n";
	}
	// User: (mu,   1, b_u, ... )
	// ItemL ( 1, b_i,   1, ... )
	inline void FixUser(UID uid) {
		auto& user = recommender->users[uid];
		user[0] = mu;
		user[1] = 1.;
	}
	inline void FixItem(IID iid) {
		auto& item = recommender->items[iid];
		item[0] = 1.;
		item[2] = 1.;
	}
	inline void FixAll() {
		for (int i = 0; i < recommender->users.size(); ++i)
			FixUser(i);
		for (int i = 0; i < recommender->items.size(); ++i)
			FixItem(i);
	}
	void RandomizeData() {
		for (auto& user: recommender->users)
			user = RandomVector(user.size());
		for (auto& item: recommender->items)
			item = RandomVector(item.size());
	}
};

int main() {

	bool model_choosing = true;
	bool testing = false;

	long long k, U, M, D, T;
	std::cin >> k >> U >> M >> D >> T;
/*	
	RecommederDataSet dataset;
	for (int i = 0; i < D; ++i) {
		int u, m, r;
		std::cin >> u >> m >> r;
		dataset.AddData(u, m, r);
	}
*/
	
	RecommederTrivialDataSet dataset;
	for (int i = 0; i < D; ++i) {
		int u, m;
		double r;
		std::cin >> u >> m >> r;
		dataset.AddData(u, m, r);
	}

	int optimal_size = 10; // Maximal size
	if (model_choosing) {
		for (const auto& i: std::vector<int>{3,10}) { // search the last non-overlearned model
			std::cerr << "Trying n = " << i << "\n";
			SVDRecommender recommender(U, M, i);
			//		SVDRecommenderTrainerStochasticGradientWithMuAndBias trainer(&recommender, &dataset);
			//		SVDRecommenderOnlineTrainer trainer(&recommender, std::cin);
			SVDRecommenderConjugateGradient trainer(&recommender, &dataset
					, 2000 /*timeup*/
					, 0.001 /*lambda*/
					, 0.1 /*regularization*/
					, 10000 * D /*max iterations*/);
			trainer.Train();
			double rmse = trainer.RMSE();
			std::cerr << "RMSE: " << rmse << "\n";
			std::cerr << "done_iterations: " << trainer.done_iterations << "\n";
			if (rmse < eps) { // overlearning
				optimal_size = std::max(3, i - 1);
				break;
			}
		}
	}
	std::cerr << "optimal_size: " << optimal_size << "\n";

	if (testing) {
		std::vector<double> regularization = { 0.0
						     , 0.00001
						     , 0.0001
						     , 0.001
						     , 0.002
						     , 0.006
						     , 0.01};
		for (const auto& r_k: regularization) {
			std::cerr << "Testing: r_k: " << r_k << "\n";
			SVDRecommender recommender(U, M, optimal_size);
			SVDRecommenderSemiOnlineTrainer trainer(&recommender, &dataset
					, 5000 /*timeup*/
					, 0.001 /*lambda*/
					, r_k /*regularization*/
					, 10000 * D /*max iterations*/);
			trainer.Train();
			double rmse = trainer.RMSE();
			std::cerr << "RMSE: " << rmse << "\n";
		}
		return 0;
	}

	SVDRecommender recommender(U, M, optimal_size);
//	SVDRecommenderTrainerStochasticGradientWithMuAndBias trainer(&recommender, &dataset);
//	SVDRecommenderOnlineTrainer trainer(&recommender, std::cin);
	SVDRecommenderConjugateGradient trainer(&recommender, &dataset
		, 50000 /*timeup*/
		, 0.002 /*lambda*/
		, 0.0 /*regularization*/
		, 10000 * D);
	trainer.Train();
#ifdef DEBUG
	double rmse = trainer.RMSE();
	std::cerr << "RMSE: " << rmse << "\n";
	std::cerr << "done_iterations: " << trainer.done_iterations << "\n";
#endif

//	for (int i = 0; i < U; ++i)
//		std::cout << "User " << i << ": " << recommender.users[i] << ";\n";
//	for (int i = 0; i < M; ++i)
//		std::cout << "Item " << i << ": " << recommender.items[i] << ";\n";

//	std::cout << "Answer: \n";
	for (int i = 0; i < T; ++i) {
		int u, m;
		std::cin >> u >> m;
//		std::cout << "U: " << u << "; M: " << m << "\n";
		std::cout << recommender.Predict(u, m) << "\n";
//		std::cout << recommender.users[u]  << "; " << recommender.items[m] << "\n";
	}
}
