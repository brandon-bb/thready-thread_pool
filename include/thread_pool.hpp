#pragma once

/**
 * @file thread_pool.hpp
 * @author Brandon Bbosa (brandon.bbosa@gmail.com)
 * @version 1.0.0
 * @date 23-02-2024
 * @copyright Copyright (c) 2024 Brandon Bbosa. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief thready::thread_pool - designing a work stealing thread pool that is compatible with C++ 20
 */

#include <thread>
#include <array> //could add a way to make the data structure used determined at compile-time
#include <queue>
#include <variant>
#include <functional>
#include <type_traits>


//making a work stealing thread_pool
namespace thready {
  template <typename... Fs>
  class thread_pool
  {
    using tasks = std::variant<std::function<Fs>...>>;

    private:
      const std::size_t min_threads_;
      const std::size_t max_threads_;

      std::queue<tasks> queue;
      std::array<std::thread, max_threads> pool;

      template <typename F>
      void execute_task (std::function<F>& task);

    public:
      explicit thread_pool ();
      explicit thread_pool (const std::size_t min_threads, const std::size_t max_threads);
      ~thread_pool ();

      void create_thread ();
      void destroy_thread ();

      //add constraint using <concepts>
      /*
      F deduces to be a reference and this violates the constraints of
      std::function as a reference is not callable, thus we remove the reference.
      To create a callable function object we bind the function to its arguments.
      To achieve move semantics to the std::function object, we utilise perfect
      forwarding.
      We do not need to create a std::variant object beforehand because it is
      constructed in-place when we push/emplace to the queue. NOTE: this is
      only viable for now using std::queue, custom queues we have not considered
      */ 
      template <typename F, typename... Args>
      void emplace_task (F&& func, Args&&... args)
      {
        queue.emplace (std::function<std::remove_reference_t<F>> 
          (std::bind(std::forward<F>(func), std::forward<Args>(args)...)));
      }

      template <typename F, typename... Args>
      void enqueue_task (F&& func, Args&&... args)
      {
        std::function<std::remove_reference_t<F>> task =
          std::bind (std::forward<F>(func), std::forward<Args>(args)...);

        queue.push (task);
      }

      


};
}